#include "app-context.hpp"

AppContext::AppContext(const std::string& conn_str)
    : db(conn_str),
      userRepository(db),
      tripRepository(db),
      createUserUseCase(userRepository),
      createTripUseCase(tripRepository),
      getTripUseCase(tripRepository),
      deleteTripUseCase(tripRepository)
{}
