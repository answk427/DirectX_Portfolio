struct BrushDesc
{
	float4 color;
	float3 center;
	float radius;
	uint shape;
	float3 padding;
};

float4 GetBrushColor(float3 pixelPosW, BrushDesc brush)
{
	if (brush.radius <= 0 || brush.shape == 0)
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	
		

	if (brush.shape == 1) //�簢�� ������ �ȼ��� �ִ���?
	{
		if ((pixelPosW.x >= (brush.center.x - brush.radius)) &&
			(pixelPosW.x <= (brush.center.x + brush.radius)) &&
			(pixelPosW.z >= (brush.center.z - brush.radius)) &&
			(pixelPosW.z <= (brush.center.z + brush.radius)))
		{
			return brush.color;
		}
	}

	
	if (brush.shape == 2) //�� �ȿ� �ȼ��� �ִ���?
	{
		float dx = pixelPosW.x - brush.center.x;
		float dz = pixelPosW.z - brush.center.z;

		float dist = sqrt(dx*dx + dz * dz);

		if (dist <= (float)brush.radius)
			return brush.color;
	}

	return float4(0.0f, 0.0f, 0.0f, 0.0f);

}