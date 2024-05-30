#ifndef MENU_H
#define MENU_H

#include "../../Utils/singleton.h"

class Menu : public CSingleton<Menu>
{
	void Login( );
	void Register( );
	void Launch( );

public:

	void Render( );

};

#endif

