 
#pragma warning( disable : 4996 ) 

 
#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include "G2D.h"
 
 
using namespace std;

// information de partie
struct GameData
{
	int idFrame   = 0;
	int HeighPix  = 800;
	int WidthPix  = 600;
	V2 BallPos    = V2(500, 30);
	V2 BallSpeed  = V2(300, 300);
	float timing;
	float T0[3] = {-1, -1, -1};
	int score = 0;
	 
	V2 LP[11] = { V2(600, 550), V2(585, 596), V2(542, 638), V2(476, 671), V2(392, 692), V2(300, 700), V2(207, 692),
		V2(123, 671), V2(57, 638), V2(14, 596), V2(0, 550) };


	V2 Bumper[3] = { V2(200, 400), V2(400, 400), V2(300, 550) };

};

GameData G;

struct Cible
{
	int a = 300;
	int a2 = 70;
	int p1 = 40;
	int p2 = 5;
	int b = 110;
	int LB[6] = {b, b + p1, b + p1 + p2, b + 2 * p1 + p2, b + 2 * p1 + 2 * p2, b + 3 * p1 + 2 * p2 };

	V2 LPC[12] = { V2(a + a2, LB[0]), V2(a + a2 + p1, LB[1]), V2(a + a2 + p1 + p2, LB[2]), V2(a + a2 + p1 + p2 + p1, LB[3]), V2(a + a2 + p1 + p2 + p1 + p2, LB[4]), V2(a + a2 + p1 + p2 + p1 + p2 + p1, LB[5]),V2(a - a2, LB[0]),
		V2(a - a2 - p1, LB[1]), V2(a - a2 - p1 - p2, LB[2]), V2(a - a2 - p1 - p2 - p1, LB[3]), V2(a - a2 - p1 - p2 - p1 - p2, LB[4]), V2(a - a2 - p1 - p2 - p1 - p2 - p1,LB[5])};

	bool VC[6] = { false, false, false, false, false, false };

};

Cible C;


V2 AllongeSegmentA(V2 A, V2 B, int r = 15)
{
	V2 T = B - A;
	T.normalize();
	V2 A2 = A - r * T;
	return A2;
}

V2 AllongeSegmentB(V2 A, V2 B, int r = 15)
{
	V2 T = B - A;
	T.normalize();
	V2 B2 = B + r * T;
	return B2;
}


bool OppositeSide(V2 A,V2 B, V2 M,V2 P)
{
	V2 AB = B-A;
	V2 AM = M-A;
	V2 AP = P-A;

	float r1 = AB^AM;
	float r2 = AB^AP;

	if (r1 >= 0 && r2 >= 0)
	{
		return false;
	}
	if (r1 <= 0 && r2 <= 0)
	{
		return false;
	}
	return true;
}

bool InterSegmentSegment(V2 E, V2 F, V2 A, V2 B)
{
	return OppositeSide(E, F, A, B) && OppositeSide(A, B, E, F);
}



void render()
{
	float dt[3] = { 1, 1, 1 };

	G2D::ClearScreen(Color::Black);
	 
	G2D::DrawStringFontMono(V2(80, G.HeighPix - 70), string("Score : ") + std::to_string(G.score), 50, 5, Color::Blue);
		 
	G2D::DrawRectangle(V2(0, 0), V2(G.WidthPix,G.HeighPix) , Color::Green );

	G2D::DrawCircle(G.BallPos, 15 , Color::Red, true);



	for (int i = 0; i < 3; i++)
		if ((G.timing - G.T0[i] )> 1)
		{
			dt[i] = 1;
		}
		else
		{
			dt[i] = G.timing - G.T0[i];
		}


	//bumper animation
	G2D::DrawCircle(V2(200, 400), 60 - (60 - 40) * dt[0], Color::Red, true);
	G2D::DrawCircle(V2(400, 400), 60 - (60 - 40) * dt[1], Color::Red, true);
	G2D::DrawCircle(V2(300, 550), 60 - (60 - 40) * dt[2], Color::Red, true);

	//bumper base
	G2D::DrawCircle(V2(200, 400), 40, Color::Blue, true);
	G2D::DrawCircle(V2(400, 400), 40, Color::Blue, true);
	G2D::DrawCircle(V2(300, 550), 40, Color::Blue, true);

	//Draw ligne dessus

	for (int i = 0; i < 10; i++)
	   G2D::DrawLine(G.LP[i],G.LP[i+1], Color::Green);

	//draw Cible

	for (int i = 0; i < 6; i++)
		if (C.VC[i] != true)
		{
			G2D::DrawLine(C.LPC[2 * i], C.LPC[2 * i + 1], Color::Red);
		}
		else
		{
			G2D::DrawLine(C.LPC[2 * i], C.LPC[2 * i + 1], Color::Green);
		}
	 

	G2D::Show();
}

V2 Rebond(V2 v, V2 n)
{
	n.normalize();
	V2 t = V2(n.y, -n.x);  // rotation de 90° du vecteur n
	float vt = v * t;   // produit scalaire, vt est un float
	float vn = v * n;    // produit scalaire, vn est un float
	V2 r = V2(vt * t.x - vn * n.x, vt * t.y - vn * n.y); // * entre un flottant et un V2
	return r;
}

void Logic()
{
	G.idFrame += 1;
	double dt = G2D::ElapsedTimeFromLastCallbackSeconds();
	G.timing = (G2D::ElapsedTimeFromStartSeconds()*1000);

	//prevision
	V2 BallNewPos = G.BallPos + G.BallSpeed * dt;
	V2 BallNewSpeed = G.BallSpeed;

	// Si collision

	if (BallNewPos.y < 15)
	{
		BallNewPos = G.BallPos;
		BallNewSpeed = Rebond(G.BallSpeed, V2(0, 800));
	}

	if (BallNewPos.y > 785)
	{
		BallNewPos = G.BallPos;
		BallNewSpeed = Rebond(G.BallSpeed, V2(0, -800));
	}

	if (BallNewPos.x < 15)
	{
		BallNewPos = G.BallPos;
		BallNewSpeed = Rebond(G.BallSpeed, V2(600, 0));
	}

	if (BallNewPos.x > 585)
	{
		BallNewPos = G.BallPos;
		BallNewSpeed = Rebond(G.BallSpeed, V2(-600, 0));
	}

	// traitement collision bumper

	for (int i = 0; i < 3; i++)
	{
		if ((BallNewPos - G.Bumper[i]).norm() <= 55)
		{
			BallNewPos = G.BallPos;
			BallNewSpeed = Rebond(G.BallSpeed, BallNewPos - G.Bumper[i]); // mettre vecteur normal des bumper pas le centre 
			G.T0[i] = G.timing;
			G.score = G.score + 100;
		}
	}

	// traitement collision cible

	V2 P = AllongeSegmentA(G.BallPos, BallNewPos);
	V2 PN = AllongeSegmentB(G.BallPos, BallNewPos);


	for (int i = 0; i < 6; i++)
	{
		if (InterSegmentSegment(C.LPC[2*i], C.LPC[2*i + 1], G.BallPos, BallNewPos) && C.VC[i]!=true)
		{
			BallNewPos = G.BallPos;
			V2 u = C.LPC[2 * i + 1] - C.LPC[2 * i];
			V2 n = V2(-(u.y), u.x);
			BallNewSpeed = Rebond(G.BallSpeed, n);
			C.VC[i] = true;
			G.score = G.score + 500;
		}
	}



	//fin du traitement des collisions
	G.BallPos = BallNewPos;
	G.BallSpeed = BallNewSpeed;

	

}



 
int main(int argc, char* argv[])
{ 
	V2 A(5, 10);
	V2 B = V2(6, 15);
	V2 AB = B - A;
	cout << AB << endl;
	//test vecteur
	V2 u(5, 7);
	V2 v(2, 3);

	V2 n(-1, 1);
	V2 v1(1, 0);
	V2 v2(1, -1);
	V2 v3(-1, 1);
	V2 v4(0, 1);
	V2 n2(0, 1);
	V2 v5(1, -1);
	V2 v6(0, -1);
	V2 v7(-1, 1);
	V2 v8(-1, -1);
	cout << "Produit scalaire  : " << (u * v) << endl;
	cout << "Produit vectoriel : " << (u ^ v) << endl;
	cout << Rebond(v1,n);
	cout << Rebond(v2, n);
	cout << Rebond(v3, n);
	cout << Rebond(v4, n);
	cout << Rebond(v5, n2);
	cout << Rebond(v6, n2);
	cout << Rebond(v7, n2);
	cout << Rebond(v8, n2);
	//fin test vecteur
	G2D::InitWindow(argc, argv, V2(G.WidthPix, G.HeighPix), V2(200,200), string("Super Flipper UwU !!"));

	G2D::Run(Logic,render);
}
 
  
 