#ifndef INCUDE_FIESEARCHERTHREAD_H
#define INCUDE_FIESEARCHERTHREAD_H



#include "Platform.h"


#include <iostream>
#include <vector>
#include <filesystem>
#include <stack>
#include <algorithm>
#include <string>
#include <thread>
#include <mutex>
#include <memory>
#include <future>

namespace fs = std::filesystem;

namespace sc
{



	class FileSearcherThread
	{
		struct MyThread
		{
			std::thread thread;
			bool is_working = 0 ;
			std::mutex is_working_mutex;
		};
	public:
		FileSearcherThread();
		~FileSearcherThread();

		void operator()(const std::string& file_name);
		void operator()(const fs::path& dir, const std::string& file_name);
		void search_file(const std::string& file_name);
		void search_file(const fs::path& dir, const std::string& file_name);

	private:

		const size_t m_max_threads;
		size_t m_checked_files;
		std::vector<MyThread> m_threads;
		std::stack<fs::path>m_dirs_to_search_in;
		std::mutex m_stack_mutex, m_output_mutex , m_finished_mutex  , m_checked_files_mutex;
		bool m_finished;



		bool get_finished();
		size_t get_size();
		void print_error(const std::error_code& error_code);
		void search_in_dir(const fs::path& dir, const std::string& file_name, int thread_index);
	};

	void recursive_search(const std::string& file_name);
}

#endif // !INCUDE_FIESEARCHERTHREAD_H

