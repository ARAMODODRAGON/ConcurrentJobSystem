
namespace cjs {
	namespace detail {
		inline std::thread::id i_main_thread_id = std::this_thread::get_id();
	}
}

inline bool cjs::is_this_main() {
	return cjs::detail::i_main_thread_id == std::this_thread::get_id();
}