#include "Activate.h"

extern Vehicle* vehicle;
void userActivate()
{
	
	static char key_buf[65] = "4542358bb38d7ba447a10abbce94a0fae352cb8b1019d13dcce5097bd2ed11c0";

  ActivateData user_act_data;
  user_act_data.ID = 1065690; /*your app ID here*/

  user_act_data.encKey = key_buf;

  vehicle->activate(&user_act_data,NULL,0);
}
