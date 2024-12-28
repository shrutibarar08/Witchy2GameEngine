#pragma once


template<typename T>
concept Requirements = requires(T x) { x.pos; };
