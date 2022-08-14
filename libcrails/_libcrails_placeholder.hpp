#pragma once
#include "logger.hpp"
#include "renderer.hpp"
#include "session_store.hpp"
#include "session_store/no_session_store.hpp"
#include "server.hpp"
#include "environment.hpp"

const std::string Crails::Renderer::default_format = "text/html";
const Crails::Logger::Symbol Crails::Logger::log_level = Crails::Logger::Error;
USE_SESSION_STORE(NoSessionStore)
void Crails::Server::initialize_request_pipe() {}
Crails::Server server(1);
namespace Crails { Environment environment = Crails::Development; }
