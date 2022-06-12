#include "FileSearcherThread.h"

namespace sc
{

    FileSearcherThread::FileSearcherThread()
        :
        m_finished(0),
        m_max_threads(7),
        m_checked_files(0),
        m_threads(m_max_threads)
    {

	}

    FileSearcherThread::~FileSearcherThread()
	{
	}

    void FileSearcherThread::operator()(const std::string& file_name)
    {
        search_file(file_name);
    }

    void FileSearcherThread::operator()(const fs::path& dir, const std::string& file_name)
    {
        search_file(dir, file_name);
    }

	void FileSearcherThread::search_file(const std::string& file_name)
    {
        search_file(ROOT, file_name);
	}

	void FileSearcherThread::search_file(const fs::path& dir, const std::string& file_name)
	{
        using namespace std::chrono_literals;

        m_dirs_to_search_in.push(dir);
        std::cout<<"Searching in "<<dir.string()<<std::endl;

        while (!get_finished())
        {
            bool all_threads_free=1;
            for (int i = 0; i < m_threads.size(); i++)
            {
                m_threads[i].is_working_mutex.lock();
                if (!m_threads[i].is_working)
                {
                    if (m_threads[i].thread.joinable())
                        m_threads[i].thread.join();
                    m_threads[i].is_working = 0;
                    if (get_size())
                    {

                        m_stack_mutex.lock();
                        auto dir = (m_dirs_to_search_in.top());
                        m_dirs_to_search_in.pop();
                        m_stack_mutex.unlock();
                        m_threads[i].is_working = 1;
                        m_threads[i].thread = std::move(std::thread(&FileSearcherThread::search_in_dir, this, dir, std::ref(file_name), i));
                        all_threads_free=0;

                    }

                }
                else
                    all_threads_free=0;
                m_threads[i].is_working_mutex.unlock();

            }

            if (all_threads_free)
                m_finished = 1;


        }
        for (auto& it : m_threads)
        {
            if (it.thread.joinable())
                it.thread.join();
        }

        std::cout << "Checked files : " << m_checked_files << std::endl;

        m_threads = std::vector<MyThread>(m_max_threads);
        m_checked_files = 0;
        m_finished = 0;
	}

    bool FileSearcherThread::get_finished()
    {
        m_finished_mutex.lock();
        auto res = m_finished;
        m_finished_mutex.unlock();
        return res;
    }

    size_t FileSearcherThread::get_size()
    {
        m_stack_mutex.lock();
        auto res = m_dirs_to_search_in.size();
        m_stack_mutex.unlock();
        return res;
    }

    void FileSearcherThread::print_error(const std::error_code& error_code)
    {
        m_output_mutex.lock();
        //std::cerr << "Error : " << error_code.value() << " , info : " << error_code.message().c_str() << std::endl;
        m_output_mutex.unlock();
    }

    void FileSearcherThread::search_in_dir(const fs::path & dir ,const std::string& file_name , int thread_index)
    {
        std::error_code error_code;


        auto dir_it = fs::directory_iterator(dir,fs::directory_options::skip_permission_denied, error_code);

        auto error_render = [&](){
                    m_threads[thread_index].is_working_mutex.lock();
                    m_threads[thread_index].is_working = 0;
                    m_threads[thread_index].is_working_mutex.unlock();};

        if (error_code.value() != 0)
        {
            print_error(error_code);
            error_render();
            return;
        }

        while (dir_it != fs::directory_iterator())
        {

            auto& it = *dir_it;

            auto status = it.status(error_code);
            if (error_code != error_code.default_error_condition())
            {
                print_error(error_code);
                break;
            }

            switch (status.type())
            {
            case fs::file_type::directory:
            {
                m_stack_mutex.lock();
                m_dirs_to_search_in.push(it.path());
                m_stack_mutex.unlock();
                break;
            }
            case fs::file_type::regular:
            {
                m_checked_files_mutex.lock();
                m_checked_files++;
                m_checked_files_mutex.unlock();

                if (it.path().filename().string() == file_name)
                {
                    m_finished_mutex.lock();
                    if(!m_finished)
                    {
                        m_output_mutex.lock();
                        std::cout << "File found, path : " << it.path().string() << std::endl;
                        m_output_mutex.unlock();

                        m_finished = 1;

                        m_threads[thread_index].is_working_mutex.lock();
                        m_threads[thread_index].is_working = 0;
                        m_threads[thread_index].is_working_mutex.unlock();
                        m_finished_mutex.unlock();
                        return;
                    }

                    m_threads[thread_index].is_working_mutex.lock();
                    m_threads[thread_index].is_working = 0;
                    m_threads[thread_index].is_working_mutex.unlock();
                    m_finished_mutex.unlock();
                    return;
                }

                break;
            }
            default:
                break;
            }

            dir_it.increment(error_code);
            if (error_code != error_code.default_error_condition())
            {
                print_error(error_code);
                break;
            }
        }

        m_threads[thread_index].is_working_mutex.lock();
        m_threads[thread_index].is_working = 0;
        m_threads[thread_index].is_working_mutex.unlock();
    }


    void recursive_search(const std::string& file_name)
    {
        size_t  files_in_root = 0;
        std::error_code error_code;
        std::stack<fs::path> dirs;
        dirs.push(ROOT);
        std::cout<<"Searching in "<<dirs.top().string()<<std::endl;
        while (dirs.size())
        {

            auto curr_dir = dirs.top();
            dirs.pop();
            auto dir_it = fs::directory_iterator(curr_dir, fs::directory_options::skip_permission_denied, error_code);
            if (error_code != error_code.default_error_condition())
            {
                std::cerr << "Error : " << error_code.value() << " , info : " << error_code.message().c_str() << std::endl;
                continue;
            }

            while (dir_it != fs::directory_iterator())
            {
                auto& it = *dir_it;
                auto status = it.status(error_code);
                if (error_code != error_code.default_error_condition())
                {
                    std::cerr << "Error : " << error_code.value() << " , info : " << error_code.message().c_str() << std::endl;
                    continue;
                }

                switch (status.type())
                {
                case fs::file_type::directory:
                {
                    dirs.push(it.path());
                    break;
                }
                case fs::file_type::regular:
                {
                    files_in_root++;
                    if (it.path().filename().string() == file_name)
                    {
                        std::cout << "File found, path : " << it.path().string() << std::endl;
                        files_in_root++;
                        std::cout << "Checked files : " << files_in_root << std::endl;
                        return;
                    }
                    break;
                }
                default:
                    break;
                }

                dir_it.increment(error_code);
                if (error_code != error_code.default_error_condition())
                {
                    std::cerr << "Error : " << error_code.value() << " , info : " << error_code.message().c_str() << std::endl;
                    continue;
                }
            }

        }
        std::cout << "Checked files : " << files_in_root << std::endl;

    }
}
