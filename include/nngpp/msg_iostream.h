#pragma once


#include <streambuf>
#include <sstream>
#include "msg_view.h"
#include "view.h"

namespace nng {
	template<class Elem, class Traits = std::char_traits<Elem>>
	class basic_msgbuf : public std::basic_streambuf<Elem, Traits>
	{
	public:
		using openmode = std::ios::openmode;
		using _super = std::basic_streambuf<Elem, Traits>;

		using char_type   = typename _super::char_type;
		using int_type    = typename _super::int_type;
		using traits_type = typename _super::traits_type;
		using pos_type    = typename _super::pos_type;
		using off_type    = typename _super::off_type;


	public:
		basic_msgbuf()  noexcept    : _msg(nullptr) {}
		~basic_msgbuf() noexcept    {close();}
		
		bool          is_open() const noexcept                             {return _msg;}
		void          close() noexcept                                     {if (is_open()) *this = basic_msgbuf();}

		/*
			Open a message for reading/writing.
		*/
		basic_msgbuf* open(nng::msg_view msg,  openmode mode)    {return open(msg .get(),     mode);}
		basic_msgbuf* open(nng::msg_body body, openmode mode)    {return open(body.get_msg(), mode);}
		basic_msgbuf* open(nng_msg*      msg,  openmode mode)    {return open_range(msg, mode, 0);}

		/*
			Open a sub-range of a message.
				byte_offset allows I/O to a later section of the body (eg, after application headers).
				bytes_max allows the readable/writable range (and hence message size) to be limited.
		*/
		basic_msgbuf* open_range(nng::msg_view msg,  openmode mode, size_t byte_offset, size_t bytes_max = ~size_t(0))    {open_range(msg.get(),      mode, byte_offset, bytes_max);}
		basic_msgbuf* open_range(nng::msg_body body, openmode mode, size_t byte_offset, size_t bytes_max = ~size_t(0))    {open_range(body.get_msg(), mode, byte_offset, bytes_max);}
		basic_msgbuf* open_range(nng_msg*      msg,  openmode mode, size_t byte_offset, size_t bytes_max = ~size_t(0))
		{
			// Validate
			if (!msg || !(mode & (std::ios::in|std::ios::out)))
				return nullptr;

			// Truncate
			if (mode & std::ios::trunc) nng_msg_realloc(msg, byte_offset);
			else
			{
				size_t len = nng_msg_len(msg);
				if (byte_offset > len) throw nng::exception(nng::error::msgsize,
					"basic_msgbuf::open_range(byte_offset > msg.len)");
			}

			// Setup
			_msg  = msg;
			_mode = mode;
			_byte_off = byte_offset;
			_byte_max = bytes_max;

			char_type *start = _start(),
				*gpos = start,
				*ppos = (((mode & std::ios::ate) | (mode & std::ios::app)) ? _g_end() : start);

			if (mode & std::ios::in)  this->setg(start, gpos, _g_end());
			if (mode & std::ios::out) this->setp(start, ppos, _p_end());

			return this;
		}

	protected:
		int sync() override
		{
			if (is_open())
			{
				_sync_length();
				return 0;
			}
			return -1;
		}

		std::streamsize xsputn(const char_type* s, std::streamsize n) override
		{
			if (!_is_writing() || !n) return 0;
			_sync_length();

			// Automatically grow message body
			size_t need_capac = _count()+n;
			if (need_capac > _capac())
			{
				if (!_auto_grow(need_capac)) return 0;
			}

			// Write data to message
			nng_msg_append(_msg, (void*) s, n*sizeof(char_type));
			this->setp(_start(), this->pptr() + n, _p_end());
			return n;
		}

		std::streamsize xsgetn(char_type* s, std::streamsize n) override
		{
			if (!_is_reading()) return 0;
			_sync_length();

			// Stop at end of data
			char_type *end = _g_end(), *pos = this->gptr();
			if (pos + n < end) n = end-pos;

			// Read data from message
			std::memcpy((void*) s, (void*) pos, sizeof(char_type)*n);
			this->setg(_start(), pos + n, _g_end());
			return n;
		}

		std::streamsize showmanyc() override
		{
			if (_is_reading())
			{
				_sync_length();
				char_type *end = _g_end(), *pos = this->gptr();
				if (pos < end) return end-pos;
			}
			return -1;
		}

		int_type underflow() override
		{
			if (_is_reading())
			{
				_sync_length();

				// See if data is available.
				char_type *gpos = this->gptr(), *gend = _g_end();
				if (gpos < gend)
				{
					this->setg(_start(), gpos, gend);
					return *gpos;
				}
			}
			
			return traits_type::eof();
		}

		int_type overflow(int_type c) override
		{
			if (!_is_writing()) return traits_type::eof();

			if (_count() >= _capac())
			{
				// Automatically grow the buffer
				if (!_auto_grow()) return traits_type::eof();
			}

			// Put character without advancing position.
			auto pptr = this->pptr();
			*this->pptr() = c;
			this->setp(_start(), this->pptr()+1, _p_end());
			return traits_type::to_int_type(c);
		}
		
		pos_type seekpos(std::streampos _pos, openmode which = std::ios::in | std::ios::out) override
		{
			_sync_length();
			size_t pos = ((_pos < 0) ? 0 : size_t(_pos));
			if (pos > _count()) pos = _count();

			if (which & std::ios::in ) this->setg(_start(), _start()+pos, _g_end());
			if (which & std::ios::out) this->setp(_start(), _start()+pos, _g_end());

			return pos;
		}

		pos_type seekoff(off_type off, std::ios::seekdir way, openmode which = std::ios::in | std::ios::out) override
		{
			char_type *gp, *pp;
			switch (way)
			{
			default:
			case std::ios::beg: gp = pp = _start(); break;
			case std::ios::end: _sync_length(); gp = pp = _g_end(); break;
			case std::ios::cur: gp = this->gptr(); pp = this->pptr(); break;
			}
			gp += off;
			pp += off;
			if (which & std::ios::in ) this->setg(_start(), gp, _g_end());
			if (which & std::ios::out) this->setp(_start(), pp, _p_end());

			if (which & std::ios::out) return pp-_start();
			else                       return gp-_start();
		}


	protected:
		nng_msg *_msg;
		openmode _mode = 0;
		size_t   _byte_off = 0, _byte_max = ~size_t(0);

		size_t     _sizeb(size_t n) const    {n -= _byte_off; return (n > _byte_max) ? _byte_max : n;}

		char_type *_start() const    {return static_cast<char_type*>(static_cast<char*>(nng_msg_body(_msg))+_byte_off);}
		char_type *_g_end() const    {return _start() + _count();}
		char_type *_p_end() const    {return _start() + _capac();}
		size_t     _capac() const    {return _sizeb(nng_msg_capacity(_msg))/sizeof(char_type);}
		size_t     _count() const    {return _sizeb(nng_msg_len     (_msg))/sizeof(char_type);}
		size_t     _limit() const    {return _byte_max                     /sizeof(char_type);}

		bool _is_writing() const noexcept    {return _mode & std::ios::out;}
		bool _is_reading() const noexcept    {return _mode & std::ios::in;}

		void _sync_length() noexcept
		{
			if (this->pptr() > _g_end())
				nng_msg_realloc(_msg, static_cast<char*>(this->pptr())-static_cast<char*>(_start()));
		}

		// Increase capacity, so that the resulting capacity is at least min_capac.
		bool _auto_grow(size_t min_capac = 32)
		{
			size_t capac = _capac();
			size_t max_capac = _limit();
			
			if (min_capac > max_capac || capac >= max_capac) return false;

			capac = capac * 2;
			if      (capac > max_capac) capac = max_capac;
			else if (capac < min_capac) capac = min_capac;
			_grow(capac);
			return true;
		}

		void _grow(size_t new_capac)
		{
			if (new_capac < _capac()) return;

			// Reallocate message and update pointers
			char_type *start;
			size_t g_ind, p_ind, written_elems = _count();
			start = _start();
			if (_is_reading()) g_ind = this->gptr() - _start();
			if (_is_writing()) p_ind = this->pptr() - _start();

			// Reserve more space
			auto code = nng_msg_reserve(_msg, new_capac * sizeof(char_type));
			if (code != 0) throw nng::exception(code, "nng_msg_realloc");

			// Update addresses
			start = _start();
			if (_is_reading()) this->setg(start, start+g_ind, _g_end());
			if (_is_writing()) this->setp(start, start+p_ind, _p_end());
		}
	};

	/*
		Implementation of istream/ostream/iostream
	*/
	template<class Elem, class Traits,
		class StreamBase,
		std::ios::openmode ModeDefault,
		std::ios::openmode ModeForce = 0>
	class basic_msgstream_ : protected basic_msgbuf<Elem, Traits>, public StreamBase
	{
	protected:
		using openmode = std::ios::openmode;
		using _mebuf = basic_msgbuf<Elem, Traits>;
		_mebuf *_asbuf() noexcept    {return static_cast<_mebuf*>(this);}


	public:
		// Open methods behave like their equivalents in msgbuf.
		void open(nng::msg_body msg, openmode mode = ModeDefault)           {_mebuf::open(msg, mode&ModeForce);}
		void open(nng::msg_view msg, openmode mode = ModeDefault)           {_mebuf::open(msg, mode&ModeForce);}
		void open(nng_msg*      msg, openmode mode = ModeDefault)           {_mebuf::open(msg, mode&ModeForce);}

		void open_range(nng::msg_body msg, openmode mode, size_t byte_offset, size_t bytes_max = ~size_t(0))           {_mebuf::open_range(msg, mode&ModeForce, byte_offset, bytes_max);}
		void open_range(nng::msg_view msg, openmode mode, size_t byte_offset, size_t bytes_max = ~size_t(0))           {_mebuf::open_range(msg, mode&ModeForce, byte_offset, bytes_max);}
		void open_range(nng_msg*      msg, openmode mode, size_t byte_offset, size_t bytes_max = ~size_t(0))           {_mebuf::open_range(msg, mode&ModeForce, byte_offset, bytes_max);}

		bool is_open() noexcept                                                       {_mebuf::is_open();}
		void close() noexcept                                                         {_mebuf::close();}

		basic_msgstream_(const _mebuf &buf)                                                                                        : StreamBase(_asbuf()), _mebuf(buf) {}
		basic_msgstream_(nng::msg_body msg, openmode mode = ModeDefault, size_t byte_offset = 0, size_t bytes_max = ~size_t(0))    : StreamBase(_asbuf()) {_mebuf::open_range(msg, mode&ModeForce, byte_offset, bytes_max);}
		basic_msgstream_(nng::msg_view msg, openmode mode = ModeDefault, size_t byte_offset = 0, size_t bytes_max = ~size_t(0))    : StreamBase(_asbuf()) {_mebuf::open_range(msg, mode&ModeForce, byte_offset, bytes_max);}
		basic_msgstream_(nng_msg*      msg, openmode mode = ModeDefault, size_t byte_offset = 0, size_t bytes_max = ~size_t(0))    : StreamBase(_asbuf()) {_mebuf::open_range(msg, mode&ModeForce, byte_offset, bytes_max);}

		basic_msgstream_()                                                            : StreamBase(_asbuf()) {}
		~basic_msgstream_() noexcept {}
		

		// Copyable, movable, etc
		basic_msgstream_(const basic_msgstream_ &o)                                   : _mebuf(o),            StreamBase(_asbuf()) {}
		basic_msgstream_(basic_msgstream_      &&o)                                   : _mebuf(std::move(o)), StreamBase(_asbuf()) {}
		basic_msgstream_ &operator=(const basic_msgstream_ &o)                        {*_asbuf() = o;            return *this;}
		basic_msgstream_ &operator=(basic_msgstream_      &&o)                        {*_asbuf() = std::move(o); return *this;}
	};


	template<class Elem, class Traits = std::char_traits<Elem>>
	using basic_msgstream = basic_msgstream_<Elem, Traits, std::basic_iostream<Elem, Traits>, std::ios::in|std::ios::out, 0>;
	template<class Elem, class Traits = std::char_traits<Elem>>
	using basic_imsgstream = basic_msgstream_<Elem, Traits, std::basic_istream<Elem, Traits>, std::ios::in,  std::ios::in>;
	template<class Elem, class Traits = std::char_traits<Elem>>
	using basic_omsgstream = basic_msgstream_<Elem, Traits, std::basic_ostream<Elem, Traits>, std::ios::out, std::ios::out>;


	/*
		msgbuf writing one byte at a time.
	*/
	using msgbuf     = basic_msgbuf<char>;
	using msgstream  = basic_msgstream<char>;
	using imsgstream = basic_imsgstream<char>;
	using omsgstream = basic_omsgstream<char>;


	/*
		Special output operators for nng::view.
	*/
	inline omsgstream &operator<<(omsgstream &out, const nng::view &v)    {out.write(v.data<char>(), v.size()); return out;}
	inline msgstream  &operator<<( msgstream &out, const nng::view &v)    {out.write(v.data<char>(), v.size()); return out;}

	template<size_t N>
	omsgstream &operator<<(omsgstream &out, const char (&s)[N])    {out.write(s, N-(s[N-1]==0)); return out;}
	template<size_t N>
	msgstream  &operator<<( msgstream &out, const char (&s)[N])    {out.write(s, N-(s[N-1]==0)); return out;}
}