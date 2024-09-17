#include <logger.h>

// static global file pointer to hold the log file
static FILE *log_file = NULL;

// get the current time, returns a char *buffer which needs to be freed
char *current_time()
{
    size_t buffer_size = 80;

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    char *buffer = (char *)malloc(buffer_size * sizeof(char));
    if (!buffer)
    {
        perror("[ERROR] Failed to allocate memory for time buffer");
        return NULL;
    }

    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", tm);
    return buffer;
}

// initialize writing to a log file
int init_logger(const char *filename)
{
    log_file = fopen(filename, "a");
    if (!log_file)
    {
        perror("[ERROR] Failed to open log file");
        return 1;
    }
    return 0;
}

// close the log file when done
void close_logger()
{
    if (log_file)
    {
        fclose(log_file);
        log_file = NULL;
    }
}

// log messages using LogLevel, and message
void log_message(LogLevel level, const char *message, ...)
{
    const char *level_str;
    switch (level)
    {
    case LOG_INFO:
        level_str = "INFO";
        break;
    case LOG_WARNING:
        level_str = "WARNING";
        break;
    case LOG_ERROR:
        level_str = "ERROR";
        break;
    default:
        level_str = "UNKNOWN";
        break;
    }

    // Prepare variadic argument list
    va_list args;
    va_start(args, message);

    // Print to console
    printf("[%s] ", level_str);
    vprintf(message, args); // Use vprintf to print the formatted message
    printf("\n");

    // Log to file if log_file is set
    if (log_file)
    {
        char *time_str = current_time();

        fprintf(log_file, "[%s] [%s] ", time_str, level_str);
        vfprintf(log_file, message, args); // Use vfprintf for file logging
        fprintf(log_file, "\n");
        fflush(log_file);

        free(time_str);
    }

    // Clean up variadic argument list
    va_end(args);
}