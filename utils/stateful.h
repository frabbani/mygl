#pragma once

#include <functional>
#include <optional>
#include <memory>

namespace utils{

template<class To, class From >
std::unique_ptr<To> unique_move( std::unique_ptr<From> from ){
  static_assert( std::is_base_of<To,From>::value || std::is_base_of<From,To>::value );
  return std::unique_ptr<To>( from.release() );
}

template<typename T>
struct StatefulState{
  T current;
  virtual void applyCb( const T& active ){}
  virtual void forceCb(){}
  virtual ~StatefulState(){}
};

template<typename T> class Stateful {
public:

  Stateful( std::unique_ptr< StatefulState<T> > initial ){
    state = std::move( initial );
    if( firstTime ){
      force();
      firstTime = false;
    }
  }

  void force(){
    state->forceCb();
    internal = state->current;
  }

  void apply(){
    if( state->current != internal ){
      state->applyCb( internal );
      internal = state->current;
    }
  }

  T& current(){ return state->current; }

private:
  static bool firstTime;
  static T internal;
  std::unique_ptr< StatefulState<T> > state;
};

template<typename T>
T Stateful<T>::internal;

template<typename T>
bool Stateful<T>::firstTime = true;

}
