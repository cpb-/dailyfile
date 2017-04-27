/// \file dailyfile.c
///
/// \brief Copy the content of its standard input into daily switching files.
///
/// \author Christophe Blaess <christophe.blaess@logilin.fr>
///
/// \licence GPLv3+
///

	#include <errno.h>
	#include <fcntl.h>
	#include <getopt.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <time.h>
	#include <unistd.h>

	#define PROGRAM_NAME            "dailyfile"

	// Get the version from the Makefile and stringinize it.
	#ifndef PROGRAM_VERSION
	  #define PROGRAM_VERSION         0
	#endif
	#define xstr(s) str(s)
        #define str(s) #s
        #define PROGRAM_VERSION_STRING  xstr(PROGRAM_VERSION)


	#define DEFAULT_BUFFER_SIZE     16384

	// Default filename is "day-YYYY-MM-DD.log"
	// with YYYY, MM and DD representing current
	// year, month and day (using G.M.Time).
	#define DEFAULT_PREFIX          "day-"
	#define DEFAULT_SUFFIX          ".log"

	// Default period for file switch in seconds
	// is one day.
	#define DEFAULT_CYCLE_DURATION  86400

int main(int argc, char *argv[])
{
	char *buffer = NULL;
	int buffer_size = DEFAULT_BUFFER_SIZE;

	char *directory = NULL;
	char *prefix    = DEFAULT_PREFIX;
	char *suffix    = DEFAULT_SUFFIX;

	int cycle_duration = DEFAULT_CYCLE_DURATION;
	int option_localtime = 0;

	int opt;

	int fd = -1;
	int err = 0;
	int len;

	time_t date;
	struct tm tm_date;

	int day = 0;
	int prev_day = 0;

	char *filename = NULL;

	// ----- Parse the command line args.

	struct option options[] = {

		{ "buffer-size", required_argument, NULL, 'b' },
		{ "cycle",       required_argument, NULL, 'c' },
		{ "directory",   required_argument, NULL, 'd' },
		{ "help",        no_argument,       NULL, 'h' },
		{ "localtime",   no_argument,       NULL, 'l' },
		{ "prefix",      required_argument, NULL, 'p' },
		{ "suffix",      required_argument, NULL, 's' },
		{ "version",     no_argument,       NULL, 'v' },
		{ NULL,          0,                 NULL,  0  }
	};

	while ((opt = getopt_long(argc, argv, "b:c:d:hlp:s:v", options, NULL)) != -1) {
		switch (opt) {
			case 'b':
				if (sscanf(optarg, "%d", &buffer_size) != 1) {
					fprintf(stderr, "%s: unable to set buffer size to %s.\n",
					        argv[0], optarg);
					exit(EXIT_FAILURE);
				}
				break;
			case 'c':
				if (sscanf(optarg, "%d", &cycle_duration) != 1) {
					fprintf(stderr, "%s: unable to set cycle duration to %s.\n",
					        argv[0], optarg);
					exit(EXIT_FAILURE);
				}
				break;
			case 'd':
				directory = optarg;
				break;
			case 'h':
				fprintf(stderr, "usage: %s [options]\n", PROGRAM_NAME);
				fprintf(stderr, "Copy its standard input into daily switching file.\n");
				fprintf(stderr, "Options:\n");
				fprintf(stderr, "  -b, --buffer-size=SIZE   Use a specific buffer size for the copy.\n");
				fprintf(stderr, "  -c, --cycle=SECONDS      Cycle duration for switching files.\n");
				fprintf(stderr, "  -d, --directory=DIR      Save the files into the given directory.\n");
				fprintf(stderr, "  -h, --help               Display this help screen and quit.\n");
				fprintf(stderr, "  -l, --localtime          Use localtime for filenames instead of G.M.Time.\n");
				fprintf(stderr, "  -p, --prefix=STRING      Use the given prefix for filenames instead of \"day-\".\n");
				fprintf(stderr, "  -s, --suffix=STRING      Use the given suffix for filenames instead of \".log\".\n");
				fprintf(stderr, "  -v, --version            Display version informations.\n");
				fprintf(stderr, "  Default filenames are \"day-YYYY-MM-DD.log\" if cycle duration is longer than a day\n");
				fprintf(stderr, "  and \"day-YYYY-MM-DD-hh-mm-ss.log\" if cycle is shorter than a day.\n");
				exit(EXIT_SUCCESS);
			case 'l':
				option_localtime = 1;
				break;
			case 'p':
				prefix = optarg;
				break;
			case 's':
				suffix = optarg;
				break;
			case 'v':
				fprintf(stderr, "%s v. %s\nCopyright 2017 Christophe Blaess\n", PROGRAM_NAME, PROGRAM_VERSION_STRING);
				fprintf(stderr, "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n");
				fprintf(stderr, "This is free software: you are free to change and redistribute it.\n");
				fprintf(stderr, "There is NO WARRANTY, to the extent permitted by law.\n");
				exit(EXIT_SUCCESS);
			default:
				fprintf(stderr, "%s: unknown option: %c.\n",
				        argv[0], opt);
				exit(EXIT_FAILURE);
		}
	}

	if ((directory != NULL) && (chdir(directory) != 0)) {
		fprintf(stderr, "%s: unable to enter directory '%s'.\n", argv[0], directory);
		err = 1;
	}

	if (!err) {
		buffer = malloc(buffer_size);
		if (buffer == NULL) {
			fprintf(stderr, "%s: Not enough memory to allocate the internal buffer.\n", argv[0]);
			err = 1;
		}
	}

	if (!err) {
		filename = malloc(strlen(prefix)+strlen(suffix) + 20);
			// strlen("YYYY-MM-DD-hh-mm-ss") = 20
		if (filename == NULL) {
			fprintf(stderr, "%s: unable to allocate memory for filename.\n", argv[0]);
			err = 1;
		}
	}

	while (!err) {
		// Read input data.
		len = read(STDIN_FILENO, buffer, buffer_size);
		if (len == 0) {
			// End of input, no error.
			break;
		}
		if (len < 0) {
			fprintf(stderr, "%s: unable to read input date (%s).\n",
			        argv[0], strerror(errno));
			err = 1;
			break;
		}

		// Check if day has changed (or file not opened yet).
		date = time(NULL);
		day = date / cycle_duration;
		if ((fd < 0) || (day != prev_day)) {
			// Open a new file.

			if (fd >= 0) // Close the previous one.
				close(fd);
			fd = -1;

			if (((option_localtime) && (localtime_r(&date, &tm_date) == NULL))
			 || ((!option_localtime) && (gmtime_r(&date, &tm_date) == NULL))) {
				fprintf(stderr, "%s: unable to parse current time.\n", argv[0]);
				err = 1;
				break;
			}
			if (cycle_duration >= 86400) {
				// One day (or more) cycles.
				sprintf(filename, "%s%04d-%02d-%02d%s",
				        prefix, tm_date.tm_year+1900,
				        tm_date.tm_mon+1, tm_date.tm_mday,
				        suffix);
			} else {
				// Cycles shorter than a day.
				sprintf(filename, "%s%04d-%02d-%02d-%02d-%02d-%02d%s",
				        prefix, tm_date.tm_year+1900,
				        tm_date.tm_mon+1, tm_date.tm_mday,
				        tm_date.tm_hour, tm_date.tm_min,
					tm_date.tm_sec, suffix);
			}
			fd = open(filename, O_WRONLY | O_CREAT, 0644);
			if (fd < 0) {
				fprintf(stderr, "%s: unable to open file '%s'.\n",
				        argv[0], filename);
				err = 1;
				break;
			}
		}

		prev_day = day;
		if (write(fd, buffer, len) != len) {
			fprintf(stderr, "%s: erreur while writing '%s' (%s).\n",
			        argv[0], filename, strerror(errno));
			err = 1;
			break;
		}
	}

	if (fd >= 0)
		close(fd);
	fd = -1;

	if (filename != NULL)
		free(filename);
	filename = NULL;

	if (buffer != NULL)
		free(buffer);
	buffer = NULL;

	return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}

