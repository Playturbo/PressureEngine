#include "WaterShader.h"

namespace Pressure {

	WaterShader::WaterShader() {
		ShaderProgram::loadShaders(PRESSURE_ENGINE_WATER_VERTEX_FILE, PRESSURE_ENGINE_WATER_FRAGMENT_FILE);
	}

	void WaterShader::bindAttributes() {
		ShaderProgram::bindAttribute(0, "position");
	}

	void WaterShader::getAllUniformLocations() {
		location_transformationMatrix = ShaderProgram::getUniformLocation("transformationMatrix");
		location_projectionMatrix = ShaderProgram::getUniformLocation("projectionMatrix");
		location_viewMatrix = ShaderProgram::getUniformLocation("viewMatrix");
		location_waveModifier = ShaderProgram::getUniformLocation("waveModifier");
		location_lightColor = ShaderProgram::getUniformLocation("lightColor");
		location_lightPosition = ShaderProgram::getUniformLocation("lightPosition");
		location_reflectionTexture = ShaderProgram::getUniformLocation("reflectionTexture");
		location_refractionTexture = ShaderProgram::getUniformLocation("refractionTexture");
		location_depthMap = ShaderProgram::getUniformLocation("depthMap");
	}

	void WaterShader::loadTransformationMatrix(Matrix4f& matrix) {
		ShaderProgram::loadMatrix(location_transformationMatrix, matrix);
	}

	void WaterShader::loadProjectionMatrix(Matrix4f& matrix) {
		ShaderProgram::loadMatrix(location_projectionMatrix, matrix);
	}

	void WaterShader::loadViewMatrix(Camera& camera) {
		ShaderProgram::loadMatrix(location_viewMatrix, Matrix4f().createViewMatrix(camera.getPosition(), camera.getPitch(), camera.getYaw(), camera.getRoll()));
	}

	void WaterShader::loadWaveModifier(float angle) {
		ShaderProgram::loadFloat(location_waveModifier, angle);
	}

	void WaterShader::loadLight(Light& light) {
		ShaderProgram::loadVector(location_lightPosition, light.getPosition());
		ShaderProgram::loadVector(location_lightColor, light.getColor());
	}

	void WaterShader::connectTextureUnits() {
		ShaderProgram::loadInt(location_reflectionTexture, 0);
		ShaderProgram::loadInt(location_refractionTexture, 1);
		ShaderProgram::loadInt(location_depthMap, 2);
	}

}