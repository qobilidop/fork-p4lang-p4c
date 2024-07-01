/* Copyright 2020-present Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef __PNA_P4__
#define __PNA_P4__

#include <core.p4>

/**
 *   P4-16 declaration of the Portable NIC Architecture
 */

#include <_internal/pna/v0_7/types_core.p4>
#include <_internal/pna/v0_7/types_defns.p4>
#include <_internal/pna/v0_7/types_defns2.p4>
#include <_internal/pna/v0_7/funcs_int_to_header.p4>
#include <_internal/pna/v0_7/types_misc.p4>

#include <_internal/pna/v0_7/extern_hash.p4>
#include <_internal/pna/v0_7/extern_checksum.p4>
#include <_internal/pna/v0_7/extern_counter.p4>
#include <_internal/pna/v0_7/extern_meter.p4>
#include <_internal/pna/v0_7/extern_register.p4>
#include <_internal/pna/v0_7/extern_random.p4>
#include <_internal/pna/v0_7/extern_action.p4>
#include <_internal/pna/v0_7/extern_digest.p4>

#include <_internal/pna/v0_7/types_metadata.p4>
#include <_internal/pna/v0_7/extern_funcs.p4>
#include <_internal/pna/v0_7/blocks.p4>

#endif   // __PNA_P4__