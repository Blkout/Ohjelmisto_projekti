#ifndef MYCONTACTLISTENER
#define MYCONTACTLISTENER

#include <string.h>
#include <cstdio>
#include <stdio.h>

// Somewhat buggy contact listener atm. think I need to make .cpp file for the code. (works tho)

class my_listener : public b2ContactListener
{
public:

void BeginContact(b2Contact* contact)
{
	if((contact->GetFixtureA()->GetBody()->GetUserData() == "box" && contact->GetFixtureB()->GetBody()->GetUserData() == "box2")
	|| contact->GetFixtureB()->GetBody()->GetUserData() == "box" && contact->GetFixtureA()->GetBody()->GetUserData() == "box2")
	{
			 printf("1 ja 2\n");
	}

	if((contact->GetFixtureA()->GetBody()->GetUserData() == "box" && contact->GetFixtureB()->GetBody()->GetUserData() == "ground")
	|| contact->GetFixtureB()->GetBody()->GetUserData() == "box" && contact->GetFixtureA()->GetBody()->GetUserData() == "ground")
	{
			 printf("1 ja ground\n");
	}

	if((contact->GetFixtureA()->GetBody()->GetUserData() == "ground" && contact->GetFixtureB()->GetBody()->GetUserData() == "box2")
	|| contact->GetFixtureB()->GetBody()->GetUserData() == "ground" && contact->GetFixtureA()->GetBody()->GetUserData() == "box2")
	{
			 printf("2 ja ground\n");
	}
}

void EndContact(b2Contact* contact)
{
	
}
void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{

}
void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	if(impulse->normalImpulses[0] > 35)
	{
		printf("voimat toimii");
	}
}
};

#endif