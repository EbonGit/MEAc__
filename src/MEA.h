#ifndef MEAC___MEA_H
#define MEAC___MEA_H

#include <mutex>
#include "Vue.h"
#include "Network.h"

class MEA : public Vue, public Network {
private :
public :
    MEA(MEA_Params params);
};

struct MEA_Info{
    MEA_Info(MEA_Params params) : params(params), mea(params) {}

    MEA_Params params;
    MEA mea;
};

#endif //MEAC___MEA_H
