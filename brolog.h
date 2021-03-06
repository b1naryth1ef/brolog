/*
This is a simple logger class that can be used to debug or log issues
*/

#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <ostream>
#include <exception>
#include <vector>

#include <string.h>
#include <stdarg.h>
#include <time.h>

// Multiwriter is a simple class that allows writing to multiple ostream objects at once.
class MultiWriter {
    private:
        std::vector<std::ostream*> mwli;

    public:
        MultiWriter() {}

        // Add an ostream pointer to the list of printables
        void add(std::ostream *f) {
            mwli.push_back(f);
        }

        // Write to the ostreams (all at once)
        void write(const char* s, std::streamsize n) {
            for ( auto &i : mwli ) {
                i->write(s, n);
            }
        }
};

class Log {
    public:
        std::ofstream *file;
        MultiWriter mw;

        Log() {
            mw.add(&std::cout);
        }

        Log(const char* f, bool use_stdout) {
            file = new std::ofstream;
            file->open(f, std::ios::out | std::ios::app);
            mw.add(file);

            if (use_stdout) {
                mw.add(&std::cout);
            }
        }

        void L(std::string tag, const char* format, ...) {
            va_list argptr;
            char buffer[2048];

            va_start(argptr, format);
            vsprintf(buffer, format, argptr);

            // Format a time string
            char timestring[10];
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            strftime(timestring, sizeof(timestring) - 1, "%H:%M:%S", t);

            // Write it out to the optional files
            mw.write("[", 1);
            mw.write(tag.c_str(), strlen(tag.c_str()));
            mw.write("] ", 2);
            mw.write(timestring, strlen(timestring));
            mw.write(" - ", 3);
            mw.write(buffer, strlen(buffer));
            mw.write("\n", 1);

            va_end(argptr);
        }

        ~Log() {
            if (file) {
                file->close();
            }
        }
};

class Exception: public std::exception {
    private:
        std::string err;

        virtual const char* what() const throw() {
            return this->err.c_str();
        }

    public:
        Exception(std::string err) {
            this->err = err;
        }
};

