#pragma once

#include <functional>
#include <git2.h>
#include <iostream>
#include <memory>

namespace fons::git
{
    template <typename T>
    class unique_libgit_ptr
    {
      public:
        constexpr unique_libgit_ptr() noexcept {};

        template <class CreateMethodT, class... CreateMethodArgs>
        unique_libgit_ptr(CreateMethodT creator, std::function<void __cdecl(T *)> free_method, CreateMethodArgs &&...args)
        {
            T *rawPtr{};
            int createResult = creator(&rawPtr, std::forward<CreateMethodArgs>(args)...);

            if (createResult != GIT_OK)
            {
                const git_error *error_details = git_error_last();

                if (error_details)
                    std::cout << error_details->klass << " :" << error_details->message << std::endl;

                return;
            }

            base_ptr = std::unique_ptr<T, std::function<void __cdecl(T *)>>{std::move(rawPtr), free_method};
        }

        T &operator*()
        {
            return &base_ptr;
        }

        T *operator->() const noexcept
        {
            return *base_ptr;
        }

        explicit operator bool() const noexcept
        {
            return static_cast<bool>(base_ptr);
        }

        auto release() noexcept
        {
            return base_ptr.release();
        }

        auto reset(std::unique_ptr<T, std::function<void __cdecl(T *)>>::pointer ptr) noexcept
        {
            return base_ptr.reset(ptr);
        }

        auto get() const noexcept
        {
            return base_ptr.get();
        }

        auto get_deleter() noexcept
        {
            return base_ptr.get_deleter();
        }

      private:
        std::unique_ptr<T, std::function<void __cdecl(T *)>> base_ptr;
    };

    using unique_commit_ptr = unique_libgit_ptr<git_commit>;
    using unique_revwalk_ptr = unique_libgit_ptr<git_revwalk>;
    using unique_git_obj_ptr = unique_libgit_ptr<git_object>;
    using unique_repo_ptr = unique_libgit_ptr<git_repository>;
    using unique_branch_itr = unique_libgit_ptr<git_branch_iterator>;
    using unique_ref_ptr = unique_libgit_ptr<git_reference>;
    using unique_tree_ptr = unique_libgit_ptr<git_tree>;
    using unique_diff_ptr = unique_libgit_ptr<git_diff>;
    using unique_patch_ptr = unique_libgit_ptr<git_patch>;
    using unique_blob_ptr = unique_libgit_ptr<git_blob>;
    using unique_id_ptr = unique_libgit_ptr<git_oid>;

    using unique_buf_ptr = std::unique_ptr<git_buf, std::function<void(git_buf *)>>;
    static std::function<void(git_buf *)> free_unique_buf = [](git_buf *buffer)
    {
        git_buf_dispose(buffer);
        delete buffer;
    };

} // namespace fons::git