#ifndef __MAP_UTILS_H__
#define __MAP_UTILS_H__

class MapUitls
{
public:
    //Template Function to clear and free memory from a map
    template <typename M> static void FreeClear(M& a_map){
        for (typename M::iterator it = a_map.begin(); it != a_map.end(); ++it) {
            delete it->second;
        }
		 a_map.clear();
	}
};


#endif __MAP_UTILS_H__