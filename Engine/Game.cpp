/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/








//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//This should be in the repo description as well in case you missed it

//Sources where I learned about the subject:

//https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/
//(Careful, on the site there are some errors made by the authour and this repo is my attempt on trying to fix the bugs in his code and
//trying to fix problems arising from my own code. There are many unneccesary steps like testing if the player is moving at all( there are
//just my attempt to optimize).

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-








#include "MainWindow.h"
#include "Game.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	playerPos(Graphics::ScreenWidth/2.0f, Graphics::ScreenHeight/2.0f),
	playerVel(0.0f, 0.0f)
{
	//Creating the borders
	for (size_t j = 0; j < blockLength; j++)
	{
		for (size_t i = 0; i < blockLength; i++)
		{
			const int id = blockLength * j + i;
			if (i == 0 || i == blockLength - 1 || j == 0 || j == blockLength - 1)
			{
				blocks[id].color = Color(255, 204, 153);
			}
			blocks[id].leftTop = Vec2(float(i * blockSide), float(j * blockSide));
			blocks[id].bottomRight = Vec2(float((i+1) * blockSide), float((j+1) * blockSide));
		}
	}
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	//Getting the delta time
	const float DT = ft.Mark();

	//World update
	if (wnd.mouse.LeftIsPressed())
	{
		const size_t id = GetBlockId(wnd.mouse.GetPos() / blockSide);

		if (blocks[id].color == Colors::Black)
		{
			blocks[id].color = Color(255, 204, 153);
		}
	}
	if (wnd.mouse.RightIsPressed())
	{
		const size_t id = GetBlockId(wnd.mouse.GetPos() / blockSide);

		if (blocks[id].color == Color(255, 204, 153))
		{
			blocks[id].color = Colors::Black;
		}
	}

	//Player update
	Controls(DT);
	Physics(DT);
}

void Game::DrawRect(Vec2 a, Vec2 b, const Color& col, bool outline)
{
	if (a.x > b.x)
	{
		std::swap(a.x, b.x);
	}
	if (a.y > b.y)
	{
		std::swap(a.y, b.y);
	}

	if (outline)
	{
		gfx.DrawLine(Vec2(a.x, a.y), Vec2(b.x - 1.0f, a.y - 1.0f), col);
		gfx.DrawLine(Vec2(a.x, b.y), Vec2(b.x - 1.0f, b.y - 1.0f), col);
		gfx.DrawLine(Vec2(a.x, a.y), Vec2(a.x-1.0f, b.y-1.0f), col);
		gfx.DrawLine(Vec2(b.x, a.y), Vec2(b.x-1.0f, b.y-1.0f), col);
	}
	else
	{
		for (int j = int(a.y); j < int(b.y); j++)
		{
			for (int i = int(a.x); i < int(b.x); i++)
			{
				if (i >= 0 && i < Graphics::ScreenWidth && j >= 0 && j < Graphics::ScreenHeight)
				{
					gfx.PutPixel(i, j, col);
				}
			}
		}
	}
}

Vei2 Game::GetPlayerBlockPos() const
{
	return GetBlockPos(GetPlayerBlockId());
}

size_t Game::GetPlayerBlockId() const
{
	return GetBlockId(Vei2(int(playerPos.x/blockSide), int(playerPos.y / blockSide)));
}

void Game::Controls(const float DT)

{
	//Side moving
	if (wnd.kbd.KeyIsPressed('A'))
	{
		playerVel.x -= playerXspeed * DT;
	}
	if (wnd.kbd.KeyIsPressed('D'))
	{
		playerVel.x += playerXspeed * DT;
	}

	//Jumping
	const size_t blockUnderPlayerId = GetPlayerBlockId() + int(blockLength * playerHeight / blockSide);
	if (blockUnderPlayerId < blockSize)
	{
		if (!(blocks[blockUnderPlayerId].color == Colors::Black))
		{
			grounded = true;
		}
		else
		{
			grounded = false;
		}
	}
	if (wnd.kbd.KeyIsPressed('W') && grounded)
	{
		playerVel.y = -playerJumpImpulse;
		grounded = false;
	}
}

std::pair<Vec2, Vec2> Game::GetBroadphaseBox(const Vec2& vel) const

{
	Vec2 leftTop(playerPos);
	Vec2 bottomRight(playerPos.x + playerWidth, playerPos.y + playerHeight);

	if (vel.x > 0.0f)
	{
		bottomRight.x += vel.x;
	}
	if (vel.x < 0.0f)
	{
		leftTop.x += vel.x;
	}
	if (vel.y > 0.0f)
	{
		bottomRight.y += vel.y;
	}
	if (vel.y < 0.0f)
	{
		leftTop.y += vel.y;
	}

	return { leftTop, bottomRight };
}

bool Game::AABB_CollisionDetection(const Vec2& leftTop0, const Vec2& bottomRight0, const Vec2& leftTop1, const Vec2& bottomRight1) const
{
	return leftTop0.x < bottomRight1.x &&
		bottomRight0.x > leftTop1.x &&
		leftTop0.y < bottomRight1.y &&
		bottomRight0.y > leftTop1.y;
}

float Game::SweptAABB(const Block& block1, const Block& block2, const Vec2& vel, Vei2& normal) const
{
	//Get the distances
	Vec2 entryDist(block2.leftTop.x - block1.bottomRight.x, block2.leftTop.y - block1.bottomRight.y);
	Vec2 exitDist(block2.bottomRight.x - block1.leftTop.x, block2.bottomRight.y - block1.leftTop.y);
	if (vel.x < 0.0f)
	{
		std::swap<float>(entryDist.x, exitDist.x);
	}
	if (vel.y < 0.0f)
	{
		std::swap<float>(entryDist.y, exitDist.y);
	}

	//Get the times
	Vec2 entryTimeV(0.0f, 0.0f);
	Vec2 exitTimeV(0.0f, 0.0f);
	if (vel.x == 0.0f)
	{
		entryTimeV.x = -std::numeric_limits<float>::infinity();
		exitTimeV.x = std::numeric_limits<float>::infinity();
	}
	else
	{
		entryTimeV.x = entryDist.x / vel.x;
		exitTimeV.x = exitDist.x / vel.x;
	}
	if (vel.y == 0.0f)
	{
		entryTimeV.y = -std::numeric_limits<float>::infinity();
		exitTimeV.y = std::numeric_limits<float>::infinity();
	}
	else
	{
		entryTimeV.y = entryDist.y / vel.y;
		exitTimeV.y = exitDist.y / vel.y;
	}

	//Getting the entry and exit time points
	const float entryTime = std::max<float>(entryTimeV.x, entryTimeV.y);
	const float exitTime = std::min<float>(exitTimeV.x, exitTimeV.y);

	//Collision test
	if (entryTime > exitTime || (entryTimeV.x < 0.0f && entryTimeV.y < 0.0f) || entryTimeV.x > 1.0f || entryTimeV.y > 1.0f)
	{
		normal.x = 0;
		normal.y = 0;
		return 1.0f;
	}
	else
	{
		if (entryTimeV.x >= entryTimeV.y)
		{
			if (entryDist.x < 0)
			{
				normal.x = 1;
				normal.y = 0;
			}
			else
			{
				normal.x = -1;
				normal.y = 0;
			}
		}
		else
		{
			if (entryDist.y < 0)
			{
				normal.x = 0;
				normal.y = 1;
			}
			else
			{
				normal.x = 0;
				normal.y = -1;
			}
		}
		return entryTime;
	}
}

void Game::CollisionTime(const Vec2& vel, std::vector<Block*>& collidableBlocks, float& collisionTime, Vei2& normal)
{
	//Find the smallest time for a possible collision and remove that block that we collided with
	size_t collidingBlockId = 0;
	for (size_t i = 0; i < collidableBlocks.size(); i++)
	{
		Vei2 n(0, 0);
		const float ct = SweptAABB(Block(playerPos, playerPos + Vec2(playerWidth, playerHeight),
			Colors::Black), *collidableBlocks[i], vel, n);

		if (ct < collisionTime)
		{
			collisionTime = ct;
			normal = n;
			collidingBlockId = i;
		}
	}
	if (!collidableBlocks.empty())
	{
		collidableBlocks.erase(collidableBlocks.begin() + collidingBlockId);
	}
}

void Game::Physics(const float DT)
{
	//Forces
	if (!grounded)
	{
		playerVel.y += gravityAcc * DT;//External forces(Ex: Gravity)
	}

	Vei2 minNormal(0, 0);
	float minCollisionTime = 1.0f;
	std::vector<Block*> collidableBlocks;

	//Integration(also has collision fixing)
	if (playerVel.x || playerVel.y)
	{
		const Vec2 playerVelFrame = playerVel * DT; //This is the velocity that we are working with, not the playerVel!

		//Broadphashing
		const auto broadphaseBox = GetBroadphaseBox(playerVelFrame);

		//Broadphase the world in this vector
		for (auto& i : blocks)
		{
			if (!(i.color == Colors::Black))
			{
				if (AABB_CollisionDetection(broadphaseBox.first, broadphaseBox.second, i.leftTop, i.bottomRight))
				{
					collidableBlocks.push_back(&i);
				}
			}
		}

		//Integration(with collisionFix if there is a collision the minCollisionTime will be between 0 and 1
		//else minCollisionTime will be 1.0f)
		CollisionTime(playerVelFrame, collidableBlocks, minCollisionTime, minNormal);
		playerPos += playerVelFrame * minCollisionTime;
	}

	//Collision response(sliding, if there was a collision)
	float remainingTime = 1.0f - minCollisionTime;
	if (remainingTime > 0.0f)
	{
		//Change the velocity for sliding
		if (minNormal.x)
		{
			playerVel.x = 0.0f;
		}
		if (minNormal.y)
		{
			playerVel.y = 0.0f;
		}

		//This part of the algorithm solves a problem where the player collision is fixed for only 1 direction but not for others
		//So if you use the simple integration here(playerPos += playerVelFrame * remainingTime) it migth slide against 
		//another wall in the direction it's sliding and will not test for collision and will move through the walls while 
		//sliding into that direction.

		//This is where we call another CollisionFix() function so it doesn't move through neither walls
		//The player must be moving against two walls at most. Why? Because at 3 or 4 walls the velocities cancel each other out so still 2 walls
		//This means we only have 4 cases, think about it, where the player is sliding horizontally and it might encounter a wall on the
		//left or right side or we might sliding vertically and it might encounter a wall on the top or bottom side.

		//At this point the player MUST BE moving horizontally, vertically or not moving at all(because we changed the velocity above)
		if (playerVel.x || playerVel.y)
		{
			//Because there is a new velocity we need to compute a new broadphase to filter the world block
			//But because we are sliding we don't have more velocity in any direction, we actually lose a velocity in a direction 
			//so it is guaranteed that the new broadphase will be smaller than the last one ===> new collidableBlocks <= old collidableBlocks
			//We don't need to do a broadphasing of the whole world, we just need to broadphase on our old collidable blocks vector

			const Vec2 playerVelFrame = playerVel * DT;

			//Broadphashing
			const auto broadphaseBox = GetBroadphaseBox(playerVelFrame);

			//Broadphase(fast erasing) the collidableBlocks vector
			const auto nEnd = std::remove_if(collidableBlocks.begin(), collidableBlocks.end(),
				[this, &broadphaseBox](const Block* block)
				{
					return !AABB_CollisionDetection(broadphaseBox.first, broadphaseBox.second, block->leftTop, block->bottomRight);
				});
			collidableBlocks.erase(nEnd, collidableBlocks.end());

			CollisionTime(playerVel * DT, collidableBlocks, remainingTime, minNormal = Vei2(0,0));
			playerPos += playerVelFrame * remainingTime;
		}
	}
}

void Game::ComposeFrame()
{
	//Drawing the world
	for (auto& i : blocks)
	{
		DrawRect(i.leftTop, i.bottomRight, i.color, false);
	}

	//Drawing the player
	if (std::any_of(std::begin(blocks), std::end(blocks), [this](const Block& b)
	{
		if (!(b.color == Colors::Black))
		{
			return AABB_CollisionDetection(playerPos, playerPos + Vec2(playerWidth, playerHeight), b.leftTop, b.bottomRight);
		}
		else
		{
			return false;
		}
	}))
	{
		DrawRect(playerPos, playerPos + Vec2(playerWidth, playerHeight), Colors::Green, false);
	}
	else
	{
		DrawRect(playerPos, playerPos + Vec2(playerWidth, playerHeight), Colors::Magenta, false);
	}
}
