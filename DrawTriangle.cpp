#include "DrawTriangle.h"

void DrawTriangle::Initialize(HINSTANCE hInstance, int width, int height)
{
}

void DrawTriangle::Destroy()
{
}

void DrawTriangle::InitTriangle()
{
	VERTEX vertices[]
	{
		{ 0.0f, 0.5f, 0.0f, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ 0.45f, -0.5f, 0.0f, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ -0.45f, -0.5f, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};
}

void DrawTriangle::InitPipeline()
{
}

void DrawTriangle::Render()
{
}
