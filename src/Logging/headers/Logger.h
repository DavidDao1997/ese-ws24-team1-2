/*
 * Logger.h
 *
 *  Created on: 27.12.2024
 *      Author: Marc
 */

#ifndef LOGGING_HEADERS_LOGGER_H_
#define LOGGING_HEADERS_LOGGER_H_

#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <fstream>

// LogLevel Definition
enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// Logger Singleton Klasse
class Logger {
public:
    // Zugriff auf die Singleton-Instanz
    static Logger& getInstance();

    // Konfiguration
    void setLogFile(const std::string& filename);
    void setLogLevel(LogLevel level, bool exclusive = false);

    // Logging Methoden
    void log(LogLevel level, const std::string& message, const std::string& className);

    // Starten und Beenden des Logging-Threads
    void start();
    void stop();

private:
    Logger();                          // Konstruktor
    ~Logger();                         // Destruktor
    Logger(const Logger&) = delete;    // Kopieren verboten
    Logger& operator=(const Logger&) = delete; // Zuweisung verboten

    // Intern
    void processLogs();                // Hintergrundprozess für Logs
    std::string formatLog(LogLevel level, const std::string& message, const std::string& className);

    // Logging-Einstellungen
    std::atomic<LogLevel> currentLogLevel;
    std::atomic<bool> exclusiveMode;

    // Log-Queue und Synchronisation
    std::queue<std::string> logQueue;
    std::mutex queueMutex;
    std::condition_variable condition;

    // Thread-Management
    std::thread loggingThread;
    std::atomic<bool> running;

    // Log-Ausgabe
    std::ofstream logFile;
};



#endif /* LOGGING_HEADERS_LOGGER_H_ */
