#include "EnginePCH.h"
#pragma hdrstop

#include "Graphics/Renderer.h"

#include "Engine.h"
#include <imgui.h>

namespace Perry
{
	static const int VALUES_COUNT = 256;
	static const float HDR_MAX = 12.0f;
	static Tonemapping* tonemapping;

	float LinearTonemapping(float HDR, float max)
	{
		if (max > 0.0f)
		{
			return glm::clamp(HDR / max, 0.0f, 1.0f);
		}
		return HDR;
	}

	float LinearTonemappingPlot(void*, int index)
	{
		return LinearTonemapping(index / (float)VALUES_COUNT * HDR_MAX, tonemapping->m_MaxLuminance);
	};

	float ReinhardTonemapping(float HDR, float k)
	{
		return HDR / (HDR + k);
	}

	float ReinhardTonemappingPlot(void*, int index)
	{
		return ReinhardTonemapping(index / (float)VALUES_COUNT * HDR_MAX, tonemapping->m_ReinhardConstant);
	}

	float ReinhardSqrTonemappingPlot(void*, int index)
	{
		float reinhard = ReinhardTonemapping(index / (float)VALUES_COUNT * HDR_MAX, tonemapping->m_ReinhardConstant);
		return reinhard * reinhard;
	}

	float ACESFilmicTonemapping(float x, float A, float B, float C, float D, float E, float F)
	{
		return (((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - (E / F));
	}

	float ACESFilmicTonemappingPlot(void*, int index)
	{
		float HDR = index / (float)VALUES_COUNT * HDR_MAX;
		return ACESFilmicTonemapping(HDR, tonemapping->m_ShoulderStrength, tonemapping->m_LinearStrength, tonemapping->m_LinearAngle, tonemapping->m_ToeStrength, tonemapping->m_ToeNumerator, tonemapping->m_ToeDenominator) /
			ACESFilmicTonemapping(tonemapping->m_LinearWhite, tonemapping->m_ShoulderStrength, tonemapping->m_LinearStrength, tonemapping->m_LinearAngle, tonemapping->m_ToeStrength, tonemapping->m_ToeNumerator, tonemapping->m_ToeDenominator);
	}

	void Renderer::ImguiDraw()
	{
		if (ImGui::BeginMainMenuBar())
		{
			ImGui::SameLine(ImGui::GetWindowWidth() - 50, 0);
			ImGui::TextColored(ImVec4(0.21f, 1.0f, 0.42f, 1.0f), "%.0f", (1000.f * GetEngine().GetDeltaTime()));
			ImGui::Text("MS");

			ImGui::EndMainMenuBar();
		}
		// if (!GetEngine().GetImguiSettings().showGraphics) return;

		//Needs to be drawn after we draw the buffers, or they are not valid
		/*
		ImGui::Begin("Buffers");
		{

#pragma warning(push)
#pragma warning(disable: 4312)
			for (int i = 0; i < std::size(m_BufferTextures); ++i)
				ImGui::Image(ImTextureID(m_BufferTextures[i]->m_TextureBuffer), ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowWidth()), ImVec2(0, 1), ImVec2(1, 0));
#pragma warning(pop)
		}
		ImGui::End();
		


		ImGui::Begin("Graphics");
		{

			ImGui::ColorEdit4("Clear Screen Color", &m_ClearColorValue.r);
			ImGui::SliderFloat("Light Intensity Factor", &m_LightIntensityFactor, 0.0f, 5.0f);

			ImGui::Text("Post Processing");
			if (ImGui::CollapsingHeader("Tonemapping", ImGuiTreeNodeFlags_None))
			{
				ImGui::SliderFloat("Exposure", &m_Tonemapping.m_Exposure, -10.0f, 10.0f);
				ImGui::SliderFloat("Gamma", &m_Tonemapping.m_Gamma, 0.01f, 5.0f);

				ImGui::Spacing();
				const char* modes[] = { "Linear", "Reinhard", "Reinhard Squared", "ACES Filmic" };
				ImGui::Combo("Tonemapping Methods", (int*)(&m_Tonemapping.tonemappingMode), modes, 4);

				tonemapping = &m_Tonemapping; // Make globally available
				switch (m_Tonemapping.tonemappingMode)
				{
				case TonemappingMode::LINEAR:
					ImGui::PlotLines("Linear Tonemapping", LinearTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 250));
					ImGui::SliderFloat("Max Brightness", &m_Tonemapping.m_MaxLuminance, 1.0f, 10.0f);
					break;
				case TonemappingMode::REINHARD:
					ImGui::PlotLines("Reinhard Tonemapping", &ReinhardTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 250));
					ImGui::SliderFloat("Reinhard Constant", &m_Tonemapping.m_ReinhardConstant, 0.01f, 10.0f);
					break;
				case TonemappingMode::REINHARDSQ:
					ImGui::PlotLines("Reinhard Squared Tonemapping", &ReinhardSqrTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 250));
					ImGui::SliderFloat("Reinhard Constant", &m_Tonemapping.m_ReinhardConstant, 0.01f, 10.0f);
					break;
				case TonemappingMode::ACES_FILMIC:
					ImGui::PlotLines("ACES Filmic Tonemapping", &ACESFilmicTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 250));
					ImGui::SliderFloat("Shoulder Strength", &m_Tonemapping.m_ShoulderStrength, 0.01f, 5.0f);
					ImGui::SliderFloat("Linear Strength", &m_Tonemapping.m_LinearStrength, 0.0f, 10.0f);
					ImGui::SliderFloat("Linear Angle", &m_Tonemapping.m_LinearAngle, 0.0f, 1.0f);
					ImGui::SliderFloat("Toe Strength", &m_Tonemapping.m_ToeStrength, 0.01f, 1.0f);
					ImGui::SliderFloat("Toe Numerator", &m_Tonemapping.m_ToeNumerator, 0.0f, 10.0f);
					ImGui::SliderFloat("Toe Denominator", &m_Tonemapping.m_ToeDenominator, 1.0f, 10.0f);
					ImGui::SliderFloat("Linear White", &m_Tonemapping.m_LinearWhite, 1.0f, 120.0f);
					break;
				default:
					break;
				}
			}

			if (ImGui::CollapsingHeader("Fog", ImGuiTreeNodeFlags_None))
			{
				ImGui::Checkbox("Enabled", &m_Fog.m_Enabled);
				if (m_Fog.m_Enabled)
				{
					const char* modes[] = { "Linear", "Exponential", "Exponential2" };
					ImGui::Combo("Fog Mode", (int*)(&m_Fog.m_FogMode), modes, 3);

					if (m_Fog.m_FogMode == FogMode::LINEAR)
					{
						ImGui::SliderFloat("Fog Start", &m_Fog.m_Start, 0.01f, 1000.0f);
						ImGui::SliderFloat("Fog End", &m_Fog.m_End, 0.01f, 3000.0f);
					}
					else
					{
						ImGui::SliderFloat("Fog Density", &m_Fog.m_Density, 0.00001f, 0.1f, "%.5f");
					}

					ImGui::Spacing();
					ImGui::ColorPicker4("Fog Color", &m_Fog.m_Color.r);
				}
			}

			if (ImGui::CollapsingHeader("SSAO", ImGuiTreeNodeFlags_None))
			{
				ImGui::Checkbox("Enabled", &m_SSAO.m_Enabled);
				if (m_SSAO.m_Enabled)
				{
					const char* modes[] = { "Low", "Medium", "High" };
					ImGui::Combo("SSAO Quality", (int*)(&m_SSAO.m_Quality), modes, 3);
					ImGui::SliderFloat("SSAO Radius", &m_SSAO.m_Radius, 0.001f, 1.0f);
					ImGui::SliderFloat("SSAO Bias", &m_SSAO.m_Bias, 0.0f, 0.8f);
					ImGui::SliderFloat("SSAO Intensity", &m_SSAO.m_Intensity, 0.0f, 5.0f);
				}
			}

			if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_None))
			{
				ImGui::Checkbox("Enabled", &m_Bloom.m_Enabled);
				if (m_Bloom.m_Enabled)
				{
					ImGui::SliderFloat("Bloom Radius", &m_Bloom.m_Radius, 0.005f, 1.0f);
					ImGui::SliderFloat("Bloom Intensity", &m_Bloom.m_Intensity, 0.0f, 4.0f);
				}
			}

			if (ImGui::CollapsingHeader("Monochromatic Effect", ImGuiTreeNodeFlags_None))
			{
				ImGui::Checkbox("Enabled", &m_MonochromaticEffect.m_Enabled);
				if (m_MonochromaticEffect.m_Enabled)
				{
					ImGui::DragFloat("Shading Threshold", &m_MonochromaticEffect.m_Threshold, 0.01f, 0.0f, 5.0f);
					ImGui::ColorPicker3("Rage Lit Color", &m_MonochromaticEffect.m_LitColor.r);
					ImGui::DragFloat("Effect Radius", &m_MonochromaticEffect.m_Radius, 1.f, 0.0f, 4000.0f);
					ImGui::ColorPicker3("Exclude Lit Color", &m_MonochromaticEffect.m_ExcludeColor.r);
					ImGui::DragFloat("Exclude Color Range", &m_MonochromaticEffect.m_ExcludeColorRange, 0.01f, 0.0f, 2.0f);
				}
			}

			if (ImGui::CollapsingHeader("Outlines", ImGuiTreeNodeFlags_None))
			{
				ImGui::Checkbox("Enabled", &m_Outlines.m_Enabled);
				if (m_Outlines.m_Enabled)
				{
					ImGui::DragFloat("Line Thickness", &m_Outlines.m_LineWidth, 0.1f, 0.0f, 10.0f);
					ImGui::DragFloat("Effect Radius", &m_MonochromaticEffect.m_Radius, 1.f, 0.0f, 4000.0f);

				}
			}
			ImGui::End();

		}*/
	}
}