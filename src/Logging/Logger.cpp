/*
 * Logger.cpp
 *
 *  Created on: 27.12.2024
 *      Author: Marc
 */


#include "headers/Logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>

// Singleton-Instanz abrufen
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

// Konstruktor
Logger::Logger() : currentLogLevel(LogLevel::INFO), exclusiveMode(false), running(false) {}

// Destruktor
Logger::~Logger() {
    stop();
}

// Log-Datei setzen
void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (logFile.is_open()) {
        logFile.close();
    }
    logFile.open(filename, std::ios::out | std::ios::app);

    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to open log file: " + filename);
    }
}

// Log-Level setzen
void Logger::setLogLevel(LogLevel level, bool exclusive) {
    currentLogLevel = level;
    exclusiveMode = exclusive;
}

// Log hinzufügen
void Logger::log(LogLevel level, const std::string& message, const std::string& className) {
    // Log-Level prüfen
    if (exclusiveMode) {
        if (level != currentLogLevel) return;
    } else {
        if (level < currentLogLevel) return;
    }

    // Log-Nachricht formatieren und in die Queue einfügen
    std::lock_guard<std::mutex> lock(queueMutex);
    logQueue.push(formatLog(level, message, className));
    condition.notify_one();
}

// Logging-Thread starten
void Logger::start() {
    running = true;
    loggingThread = std::thread(&Logger::processLogs, this);
}

// Logging-Thread stoppen
void Logger::stop() {
    if (!running) return;
    running = false;
    condition.notify_one();
    if (loggingThread.joinable()) {
        loggingThread.join();
    }
    if (logFile.is_open()) {
        logFile.close();
    }
}

// Hintergrundprozess für Logs
void Logger::processLogs() {
    while (running || !logQueue.empty()) {
        std::unique_lock<std::mutex> lock(queueMutex);
        condition.wait(lock, [this] { return !logQueue.empty() || !running; });

        while (!logQueue.empty()) {
            const std::string logEntry = logQueue.front();
            logQueue.pop();

            // Ausgabe in die Datei oder Konsole
            // if (logFile.is_open()) {
            //     logFile << logEntry << std::endl;
            // } else {
            //     std::cerr << logEntry << std::endl;
            // }
            std::cerr << logEntry << std::endl;
        }
    }
}

// Log formatieren
std::string Logger::formatLog(LogLevel level, const std::string& message, const std::string& className) {
    // Zeitstempel generieren
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << "[" << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << nowMs.count() << "] ";

    // Log-Level hinzufügen
    switch (level) {
        case LogLevel::DEBUG:   oss << "[DEBUG]   "; break;
        case LogLevel::INFO:    oss << "[INFO]    "; break;
        case LogLevel::WARNING: oss << "[WARNING] "; break;
        case LogLevel::ERROR:   oss << "[ERROR]   "; break;
        case LogLevel::CRITICAL:oss << "[CRITICAL]"; break;
    }

    // Klassennamen und Nachricht hinzufügen
    oss << " [" << className << "] " << message;
    return oss.str();
}

