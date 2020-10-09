/**
\brief Initializes the CoAP application layer and its security extensions.

\author Timothy Claeys <timothy.claeys@inria.fr>, March 2020.
*/

#include "web.h"

#if OPENWSN_COAP_C
#include "coap.h"
#endif

void web_init(void) {

#if OPENWSN_COAP_C
    coap_init();
#endif
}
