#ifndef OT_UTILITY_SCOPE_GUARD_HPP_
#define OT_UTILITY_SCOPE_GUARD_HPP_

namespace ot {

template <typename F>
class ScopeGuard {
  
  private:
    
    F _f;
    bool _active;

  public:

    ScopeGuard(F&& f) : _f {std::forward<F>(f)}, _active {true} {
    }

    ~ScopeGuard() {
      if(_active) _f();
    }

    void dismiss() noexcept {
      _active = false;
    }

    ScopeGuard() = delete;
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator = (const ScopeGuard&) = delete;
    ScopeGuard& operator = (ScopeGuard&&) = delete;

    ScopeGuard(ScopeGuard&& rhs) : _f{std::move(rhs._f)}, _active {rhs._active} {
      rhs.dismiss();
    }
};

template <typename F> ScopeGuard(F&&) -> ScopeGuard<F>;

template <typename F>
auto make_scope_guard(F&& f) {
  return ScopeGuard<F>(std::forward<F>(f));
}


};  // end of namespace dtc. ----------------------------------------------------------------------



#endif
