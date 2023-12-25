
cbuffer CB1 : register(b0)
{
    int MouseX;
    int MouseY;
};

struct CS_OUTPUT
{
    uint id;
};

Texture2D<uint> EditorIdsBuffer : register(t0);
RWStructuredBuffer<CS_OUTPUT> IdOut : register(u0);

[numthreads(1, 1, 1)]
void CSMain(void)
{
    IdOut[0].id = EditorIdsBuffer.Load(int3(MouseX, MouseY, 0));
}
