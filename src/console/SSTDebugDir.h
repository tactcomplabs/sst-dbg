//
// _SSTDebugDir_h_
//
// Copyright (C) 2017-2023 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <set>
#include <vector>
#include <thread>
#include <mutex>

class DumpDir {
public:
    DumpDir(const std::string& directory, const std::string& file_type) :
        m_dir(directory), m_file_type(file_type) {
        m_running = true;
        m_update_thread = std::thread(&DumpDir::update_files, this);
    }

    ~DumpDir() {
        m_running = false;
        if (m_update_thread.joinable()) {
            m_update_thread.join();
        }
    }

    void PrintFiles() {
        std::lock_guard<std::mutex> lock(m_files_mutex);
        for (const auto& file : m_files) {
            std::cout << file << std::endl;
        }
    }
    std::string ProcessFile(std::string Filename){
      std::lock_guard<std::mutex> lock(m_unprocessed_files_mutex);
      m_unprocessed_files.erase(std::remove(m_unprocessed_files.begin(),
                                            m_unprocessed_files.end(),
                                            Filename), m_unprocessed_files.end());
      return Filename;
    }

    const std::vector<std::string>& get_files() {
        std::lock_guard<std::mutex> lock(m_files_mutex);
        return m_files;
    }

    const std::vector<std::string>& get_unprocessed_files() {
      std::lock_guard<std::mutex> lock(m_unprocessed_files_mutex);
      return m_unprocessed_files;
    }

private:
    void update_files() {
        std::set<std::filesystem::path> previous_files;
        while (m_running) {
            std::set<std::filesystem::path> current_files;

            for (const auto& entry : std::filesystem::directory_iterator(m_dir)) {
                if (entry.path().extension() == m_file_type) {
                    current_files.insert(entry.path());
                }
            }

            std::vector<std::string> new_files;
            for (const auto& file : current_files) {
                if (previous_files.find(file) == previous_files.end()) {
                    new_files.push_back(file.string());
                }
            }

            // new files found
            if (!new_files.empty()) {
                std::lock_guard<std::mutex> lock(m_files_mutex);
                m_files.insert(m_files.end(), new_files.begin(), new_files.end());
                m_unprocessed_files.insert(m_unprocessed_files.end(),
                                           new_files.begin(),
                                           new_files.end());
            }

            previous_files = current_files;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }


    std::string m_dir;
    std::string m_file_type;
    std::vector<std::string> m_files;
    std::vector<std::string> m_unprocessed_files;
    bool m_new_files_found;
    std::thread m_update_thread;
    bool m_running;
    std::mutex m_unprocessed_files_mutex;
    std::mutex m_files_mutex;
};
