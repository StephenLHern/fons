#pragma once

#include <functional>
#include <git2.h>
#include <memory>

namespace fons::git
{
    template <typename T>
    class unique_libgit_ptr
    {
      public:
        template <class CreateMethodT, class... CreateMethodArgs>
        unique_libgit_ptr(CreateMethodT creator, std::function<void __cdecl(T *)> free_method, CreateMethodArgs &&...args)
        {
            T *rawPtr{};
            int createResult = creator(&rawPtr, std::forward<CreateMethodArgs>(args)...);

            if (createResult != GIT_OK)
                return;

            base_ptr = std::unique_ptr<T, std::function<void __cdecl(T *)>>{std::move(rawPtr), free_method};
        }

        T &operator*()
        {
            return &base_ptr;
        }

        T *operator->()
        {
            return *base_ptr;
        }

        operator bool()
        {
            return static_cast<bool>(base_ptr);
        }

        auto release()
        {
            return base_ptr.release();
        }

        auto reset(std::unique_ptr<T, std::function<void __cdecl(T *)>>::pointer ptr)
        {
            return base_ptr.reset(ptr);
        }

        auto get()
        {
            return base_ptr.get();
        }

        auto get_deleter()
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

} // namespace fons::git