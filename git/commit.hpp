#pragma once

namespace fons::git
{
    class commit
    {
      public:
        commit() : time_stamp(0)
        {
        }

        commit(std::string input_id, std::string input_message, std::string input_author_name, std::string input_author_email,
               __time64_t input_time_stamp)
            : id(input_id), message(input_message), author_name(input_author_name), author_email(input_author_email),
              time_stamp(input_time_stamp)
        {
        }

        commit(const commit &copy)
        {
            id = copy.id;
            message = copy.message;
            author_name = copy.author_name;
            author_email = copy.author_email;
            time_stamp = copy.time_stamp;
        }

        std::string id;
        std::string message;
        std::string author_name;
        std::string author_email;
        __time64_t time_stamp;
    };
} // namespace fons::git