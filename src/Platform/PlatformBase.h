#pragma once

#include <string>
#include <vector>
#include "Backends/PlatformBackend.h"

class PlatformBase {
public:
    class FileDialogBase {
    public:
        // For file filters inside of file prompts
        struct FileFilter {
            std::string label;
            std::string pattern;
        };

        // For buttons inside of prompts
        struct MessageButton {
            int flags;
            int id;
            std::string text;
        };

        virtual ~FileDialogBase() = default;

        // Open file from disk
        virtual std::string OpenFile(const std::vector<FileFilter>& filters) = 0;

        // Save file to disk
        virtual std::string SaveFile(const std::vector<FileFilter>& filters, const std::string& defaultName = "") = 0;

        // Choose folder from disk
        virtual std::string ChooseFolder() = 0;

        // Show common error prompt
        virtual void ShowError(const std::string& title, const std::string& message) = 0;

        // Show common information prompt
        virtual void ShowInfo(const std::string& title, const std::string& message) = 0;
    };

    // Show basic message prompt, extendable with buttons
    virtual int ShowMessagePrompt(const char* title, const char* message,
        const std::vector<FileDialogBase::MessageButton>& buttons) = 0;

    // Show common success prompt
    virtual void ShowSuccessMessage() = 0;

    // Show common cancellation prompt
    virtual void ShowCancelledMessage() = 0;

    // Show common yes/no Message prompt
    virtual bool ShowYesNoMessagePrompt(const char* title, const char* message) = 0;

    virtual ~PlatformBase() {}

    // Initialize platform framework
    virtual bool Init(const char* title, int width, int height) = 0;

    // Process platform framework events
    virtual void PollEvents(PlatformBackend* backend) = 0;

    // Checks if application should close
    virtual bool ShouldClose() const = 0;

    // Runs the shutdown/clean up event
    virtual void Shutdown() = 0;
};