#include <yart.h>

using namespace yart;

// int main(int argc, char** argv)
//{
//	yart::Initialize();
//
//	const i32 x = 32, y = 32, grid = 32;
//
//	Bounds2f cropWindow{{0, 0}, {1, 1}};
//	Vector2i resolution{x, y};
//	Scope<Filter> filter = CreateScope<TriangleFilter>(Vector2f{5, 5});
//
//	Film film(resolution, cropWindow, std::move(filter), 0.1, "testnorm.exr", 1);
//
//	for (i32 j = 0; j < y; j++)
//	{
//		for (i32 i = 0; i < x; i++)
//		{
//			film.AddSample(Vector2f(i, j), RGBSpectrum::FromRGB({0, 0, 0}));
//		}
//	}
//
//	film.AddSample(Vector2f(16, 16), RGBSpectrum::FromRGB({50, 0, 0}));
//
//	// for (i32 j = 0; j < y; j++)
//	//{
//	//	for (i32 i = 0; i < x; i++)
//	//	{
//	//		RGBSpectrum L;
//	//		if (((i / grid + j / grid)) % 2 == 0)
//	//		{
//	//			L = RGBSpectrum::FromRGB({1, 0, 0});
//	//		}
//	//		else
//	//		{
//	//			L = RGBSpectrum::FromRGB({0, 1, 0});
//	//		}
//	//		film.AddSample(Vector2f(i, j), L);
//	//	}
//	//}
//
//	film.WriteImage(true);
//
//	return 0;
//}

template <typename Spectrum>
class TestIntegrator final : public SamplerIntegrator<Spectrum>
{
public:
	using Scene = yart::Scene<Spectrum>;

	TestIntegrator(Ref<AbstractCamera> camera, Ref<AbstractSampler> sampler) : SamplerIntegrator<Spectrum>(camera, sampler)
	{
	}

	virtual Spectrum Li(const Ray& ray, const Scene& scene, AbstractSampler& sampler, MemoryArena& arena,
						i32 depth = 0) const override
	{
		if (scene.IntersectRay(ray))
			return Spectrum::FromRGB({1, 0, 0});
		else
			return Spectrum::FromRGB({0, 0, 0});
	}
};

int main(int argc, char** argv)
{
	using AbstractGeometry = yart::AbstractGeometry<RGBSpectrum>;
	using GeometricPrimitive = yart::GeometricPrimitive<RGBSpectrum>;
	using AbstractPrimitive = yart::AbstractPrimitive<RGBSpectrum>;
	using Sphere = yart::Sphere<RGBSpectrum>;
	using Scene = yart::Scene<RGBSpectrum>;

	yart::Initialize();
	const i32 x = 320, y = 320, spp = 20;

	Bounds2f cropWindow{{0, 0}, {1, 1}};
	Vector2i resolution{x, y};
	Scope<Filter> filter = CreateScope<BoxFilter>(Vector2f{0.5, 0.5});

	Film film(resolution, cropWindow, std::move(filter), 0.1, "test_image.exr", 1);
	Bounds2f screenWindow({-5, -5}, {5, 5});
	Ref<AbstractCamera> camera = CreateRef<OrthographicCamera>(Transform{}, screenWindow, &film);
	Ref<AbstractSampler> sampler = CreateRef<IndependentSampler>(spp, 0);

	Transform model = Translate({0, 0, 5});
	Transform invModel = Inverse(model);
	real radius = 2.5;
	Ref<AbstractGeometry> sphereGeometry = CreateRef<Sphere>(&model, &invModel, false, radius, -radius, radius, 360);
	Ref<AbstractPrimitive> spherePrimitive = CreateRef<GeometricPrimitive>(sphereGeometry);
	Scene scene(spherePrimitive);

	TestIntegrator<RGBSpectrum> integrator(camera, sampler);
	integrator.Render(scene);

	return 0;
}