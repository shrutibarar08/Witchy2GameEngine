#pragma once


template<typename T>
concept RequirePos = requires(T x) { x.pos; };

template<typename T>
concept RequireTex = requires(T x) { x.tex; };

template<typename T>
concept RequireNorm = requires(T x) { x.norm; };

template<typename T>
concept RequireColor = requires(T x) { x.color; };
