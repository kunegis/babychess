#ifndef LABEL_HH
#define LABEL_HH

#include "widget.hh"
#include "string.hh"

class Label
	:  public Widget
{
public:

	struct Open_Info
	{
		String text;
		float alignment;
		String span_attributes;
		bool with_mnemonic; 
	};

	bool Open(Pointer <Widget> parent,
		  String new_text= "",
		  float alignment= 0.0,
		  String new_span_attributes= "",
		  bool new_with_mnemonic= false)
		{
			Open_Info oi= {new_text, 
				       alignment,
				       new_span_attributes,
				       new_with_mnemonic};
			return Open(parent, oi);
		}

	bool Open(Pointer <Widget> parent,
		  const Open_Info &open_info);

	String Text() const;
  
	void Set(String text);
	/* Use current span attributes.  
	 * If span_attributes begins with <, markup is contained in
	 * text. 
	 */

private:
	String span_attributes;
	bool with_mnemonic;
};

class Title_Label
	:  public Label
{
public:
	bool Open(Pointer <Widget> parent,
		  const char *title)
		/* TITLE must be already translated */
		{
			return Label::Open
				(parent, 
				 title, 0.0,
				 "weight = \"bold\"");
		}
};

#endif /* ! LABEL_HH */
