/**
  * @file falcon_ec.h
  *
  */
#pragma once

#include "quantum.h"

#define LAYOUT_default( \
    K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112, K113, K114, K115, \
    K201, K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K214, \
    K301, K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312,       K313, \
    K401,       K402, K403, K404, K405, K406, K407, K408, K409, K410, K411, K412, K413, \
          K502, K503,                   K506,                   K508, K509 \
) {\
    {K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112, K113, K114, K115}, \
    {K201, K202, K203, K204, K205, K206, K306, K207, K208, K209, K210, K211, K212, K213, K214}, \
    {K301, K302, K303, K304, K404, K405, K406, K307, K308, K309, K310, K311, K312, K313, K413}, \
    {K401, K502, K402, K403, K503, K305, K506, K407, K408, K409, K410, K411, K508, K412, K509}, \
}
