#pragma once



#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<function/log.h>
#include<math/vector.h>
#include<memory>

namespace ls
{
	template<typename Ori,typename Dest>
	Dest* safePtrCast(Ori* ptr)
	{
		ls_AssertMsg(typeid(*ptr) == typeid(Dest), "Unexpected cast error encountered ");
		return static_cast<Dest*>(ptr);
	}

	template<typename Ori,typename Dest>
	std::shared_ptr<Dest> safeSmartPtrCast(const std::shared_ptr<Ori>& ptr)
	{
		ls_AssertMsg(typeid(*ptr) == typeid(Dest), "Unexpected cast error encountered ");
		return std::static_pointer_cast<Dest>(ptr);
	}


	ls_ForceInline void clearMemory(void* p, u32 size)
	{
		std::memset(p, 0, size);
	}
	
	ls_ForceInline std::string toString(const std::wstring& wstr)
	{
		using namespace std;
		string str;
		locale loc = locale();
		for (auto const& c : wstr)
		{
			char ch = std::use_facet<ctype<wchar_t>>(loc).narrow(c);
			str.push_back(ch);
		}
		return str;
	}

	ls_ForceInline std::wstring toWString(const std::string& str)
	{
		using namespace std;
		wstring wstr;
		locale loc = locale();
		for (auto const& c : str)
		{
			wchar_t wchar = std::use_facet<ctype<wchar_t>>(loc).widen(c);
			wstr.push_back(wchar);
		}

		return wstr;
	}



	template <typename Predicate>
	int FindInterval(int size, const Predicate &pred)
	{
		int first = 0, len = size;
		while (len > 0) {
			int half = len >> 1, middle = first + half;
			// Bisect range based on value of _pred_ at _middle_
			if (pred(middle)) {
				first = middle + 1;
				len -= half + 1;
			}
			else
				len = half;
		}
		return lsMath::clamp(first - 1, 0, size - 2);
	}

	/*
		Distribution1D & Distribution2D are both from PBRT-V3
	*/

	class Distribution1D 
	{

	public:

		// Distribution1D Public Methods
		Distribution1D(const float *f, int n) : func(f, f + n), cdf(n + 1) {
			// Compute integral of step function at $x_i$
			cdf[0] = 0;
			for (int i = 1; i < n + 1; ++i) cdf[i] = cdf[i - 1] + func[i - 1] / n;

			// Transform step function integral into CDF
			funcInt = cdf[n];
			if (funcInt == 0) {
				for (int i = 1; i < n + 1; ++i) cdf[i] = float(i) / float(n);
			}
			else {
				for (int i = 1; i < n + 1; ++i) cdf[i] /= funcInt;
			}
		}
		int Count() const { return func.size(); }
		float SampleContinuous(float u, float *pdf, int *off = nullptr) const {
			// Find surrounding CDF segments and _offset_
			int offset = FindInterval(cdf.size(),
				[&](int index) { return cdf[index] <= u; });
			if (off) *off = offset;
			// Compute offset along CDF segment
			float du = u - cdf[offset];
			if ((cdf[offset + 1] - cdf[offset]) > 0) {
				du /= (cdf[offset + 1] - cdf[offset]);
			}


			// Compute PDF for sampled offset
			if (pdf) *pdf = (funcInt > 0) ? func[offset] / funcInt : 0;

			// Return $x\in{}[0,1)$ corresponding to sample
			return (offset + du) / Count();
		}

		int SampleDiscrete(float u, float *pdf = nullptr,
			float *uRemapped = nullptr) const 
		{
			// Find surrounding CDF segments and _offset_
			int offset = FindInterval(cdf.size(),
				[&](int index) { return cdf[index] <= u; });
			if (pdf) *pdf = (funcInt > 0) ? func[offset] / (funcInt * Count()) : 0;
			if (uRemapped)
				*uRemapped = (u - cdf[offset]) / (cdf[offset + 1] - cdf[offset]);
			return offset;
		}
		float DiscretePDF(int index) const {
			return func[index] / (funcInt * Count());
		}

		// Distribution1D Public Data
		std::vector<float> func, cdf;
		float funcInt;
	};

	class Distribution2D {
	public:
		// Distribution2D Public Methods
		Distribution2D(const f32 *data, int nu, int nv);
		Point2 SampleContinuous(const Point2 &u, f32 *pdf) const {
			f32 pdfs[2];
			int v;
			f32 d1 = pMarginal->SampleContinuous(u.y, &pdfs[1], &v);
			f32 d0 = pConditionalV[v]->SampleContinuous(u.x, &pdfs[0]);
			*pdf = pdfs[0] * pdfs[1];
			return Point2(d0, d1);
		}
		f32 Pdf(const Point2 &p) const {
			int iu = lsMath::clamp(int(p.x * pConditionalV[0]->Count()), 0,
				pConditionalV[0]->Count() - 1);
			int iv =
				lsMath::clamp(int(p.y * pMarginal->Count()), 0, pMarginal->Count() - 1);
			return pConditionalV[iv]->func[iu] / pMarginal->funcInt;
		}

	private:
		// Distribution2D Private Data
		std::vector<std::unique_ptr<Distribution1D>> pConditionalV;
		std::unique_ptr<Distribution1D> pMarginal;
	};

	struct MonteCarlo
	{
	public:
		static void sampleTriangle(
			ls_Param_In Point2 uv,
			ls_Param_Out Point2* b
		);

		static void sampleHemisphere(ls_Param_In Point2 uv,
			ls_Param_Out Vec3* w,
			ls_Param_Out f32* pdf);
		static f32  sampleHemispherePdf(const Vec3& w);
		static void sampleCosHemisphere(ls_Param_In Point2 uv,
			ls_Param_Out Vec3* w,
			ls_Param_Out f32* pdf);
		static f32 sampleCosHemispherePdf(const Vec3& w);

		static void sampleSphere(ls_Param_In Point2 uv,
			ls_Param_Out Vec3* w,
			ls_Param_Out f32* pdf);
		static f32 sampleSpherePdf(const Vec3& w);

		static void sampleDisk(ls_Param_In Point2 uv,
			ls_Param_Out Point2* p,
			ls_Param_Out f32* pdf);
		static f32 sampleDistPdf(const Point2& p);

		static void sampleConcentricDisk(ls_Param_In Point2 uv,
			ls_Param_Out Point2* p,
			ls_Param_Out f32* pdf);
		static f32 sampleConcentricDistPdf(const Point2& p);
		
		static void sampleBeckmanDistributionAll(ls_Param_In Point2 uv,
			ls_Param_In f32 alphaU,
			ls_Param_In f32 alphaV,
			ls_Param_Out Vec3* wh,
			ls_Param_Out f32* pdf);
		static void sampleBeckmanDistributionVisible(ls_Param_In Point2 uv,
			ls_Param_In f32 alphaU,
			ls_Param_In f32 alphaV,
			ls_Param_In const Vec3& w,
			ls_Param_Out Vec3* wh,
			ls_Param_Out f32* pdf);

		static void sampleGGXDistributionAll(ls_Param_In Point2 uv,
			ls_Param_In f32 alphaU,
			ls_Param_In f32 alphaV,
			ls_Param_Out Vec3* wh,
			ls_Param_Out f32* pdf);

		static void sampleGGXDistributionVisible(ls_Param_In Point2 uv,
			ls_Param_In f32 alphaU,
			ls_Param_In f32 alphaV,
			ls_Param_In const Vec3& w,
			ls_Param_Out Vec3* wh,
			ls_Param_Out f32* pdf);

		static f32 beckmanDistributionAllPdf(const Vec3& w,
			const Vec3& wh,
			f32 alphaU, f32 alphaV);
		static f32 beckmanDistributionVisiblePdf(const Vec3& w,
			const Vec3& wh,
			f32 alphaU, f32 alphaV);
		static f32 ggxDistributionAllPdf(const Vec3& w,
			const Vec3& wh,
			f32 alphaU, f32 alphaV);
		static f32 ggxDistributionVisiblePdf(const Vec3& w,
			const Vec3& wh,
			f32 alphaU, f32 alphaV);

	};

	struct RenderLib
	{
	public:
		static f32 mis(s32 nf, f32 pf, s32 ng, f32 pg);
		static f32 mis(f32 pf, f32 pg);
		static f32 pdfW2A(f32 pdfW, f32 r, f32 cos);
		static f32 pdfA2W(f32 pdfA, f32 r, f32 cos);
		static bool visible(ScenePtr scene, Point p0, Point p1,f32 time = 0.f);
		static bool globalSample(ls_Param_In ScenePtr scene, ls_Param_In SamplerPtr sampler,
			ls_Param_Out MeshSampleRecord* meshRec);
		static bool matchScatterFlag(
			ls_Param_In ScatteringFunctionPtr scatter,
			ls_Param_In u32	flag);

		static void fillScatteringRecordForBSDFValueAndPdf(
			ls_Param_In const Point3& pos,
			ls_Param_In const Normal& normal,
			ls_Param_In const Vec3& wo,
			ls_Param_In const Vec3& wi,
			ls_Param_In u32 mode,
			ls_Param_In ls_Param_Out ScatteringRecord*sr);

		static void fillScatteringRecordForBSDFSample(
			ls_Param_In const Point3& pos,
			ls_Param_In const Normal& normal,
			ls_Param_In const Vec3& wo,
			ls_Param_In u32 mode,
			ls_Param_In ls_Param_Out ScatteringRecord*sr);

		static void surfaceBSDFValueAndPdf(
			ls_Param_In ScatteringFunctionPtr bsdf, 
			ls_Param_In ls_Param_Out ScatteringRecord* sr
		);

		static void sampleSurfaceBSDF(
			ls_Param_In SamplerPtr		sampler,
			ls_Param_In ScatteringFunctionPtr bsdf,
			ls_Param_In ls_Param_Out ScatteringRecord * sr);

		static f32 snellLaw(
			ls_Param_In f32 etaI,
			ls_Param_In f32 cosThetaI,
			ls_Param_In f32 etaT);

		static f32 correctShadingNormal(
			ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wo,
			ls_Param_In const Normal& ng,
			ls_Param_In const Normal& ns,
			ls_Param_In s32 transMode);


		static f32 fresnelDielectric(f32 cosThetaI, f32 etaI,
			f32 cosThetaT, f32 etaT);

		static f32 fresnelConductor(f32 cosThetaI, f32 etaI,
			f32 etaT, f32 k);

		static Spectrum fresnelConductor(f32 cosThetaI, f32 etaI,
			Spectrum etaT, Spectrum k);

		
		//Microfacets functions are from Mitsuba0.5
		static f32 beckmanG1(const Vec3& v, const Vec3& m,f32 alpha);
		static f32 ggxG1(const Vec3& v, const Vec3& m, f32 alpha);
		static f32 beckmanDistribution(const Vec3& wh,
			f32 alphaU,
			f32 alphaV);
		static f32 ggxDistribution(const Vec3& wh,
			f32 alphaU,
			f32 alphaV);
		


		

	};

	struct GeometryLib
	{
	public:
		static RTCRay lsRay2Embree(const Ray& ray);
	};
	

}