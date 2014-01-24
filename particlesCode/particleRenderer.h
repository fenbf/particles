class ParticleSystem;

class ParticleRenderer
{
public:
	ParticleRenderer() { }
	virtual ~ParticleRenderer() { }

	virtual void generate(ParticleSystem *sys, bool useQuads) = 0;
	virtual void destroy() = 0;
	virtual void update() = 0;
	virtual void render() = 0;
};

class GLParticleRenderer : ParticleRenderer
{
protected:
	ParticleSystem *m_system{ nullptr };

	unsigned int m_bufPos{ 0 };
	unsigned int m_bufCol{ 0 };
	unsigned int m_vao{ 0 };
public:
	GLParticleRenderer() { }
	~GLParticleRenderer() { destroy(); }

	void generate(ParticleSystem *sys, bool useQuads) override;
	void destroy() override;
	void update() override;
	void render() override;
};