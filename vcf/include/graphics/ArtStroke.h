//ArtStroke.h
#ifndef _ARTSTROKE_H__
#define _ARTSTROKE_H__

namespace VCF {

	class GRAPHICSKIT_API ArtStroke : public Object, public Stroke {
	public:
		ArtStroke();

		virtual ~ArtStroke();

		void init();

		virtual void setContext( GraphicsContext* context ){
			m_context = context;
		}
		
		virtual void render( Path * path );
		
		double getWidth(){
			return 	m_width;
		}
		
		void setWidth( double width ) {
			m_width = width;
		}
		
		Color* getColor() {
			return &m_color;
		}
		
		void setColor( Color* color ){
			m_color = *color;
		}

	protected:
		double m_width;
		Color m_color;
		GraphicsContext* m_context;
	};
};


#endif //_ARTSTROKE_H__
