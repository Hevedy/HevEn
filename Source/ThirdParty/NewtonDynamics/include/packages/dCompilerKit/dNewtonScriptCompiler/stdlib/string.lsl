/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

// standard library core support
class string
{
	string ()
    {
    }

	string (byte[] data)
    {
        m_data = new init (data);
    }
	
    // public interface to native code
    native int lenght() const;

    // private interface to native code
    private native void[] init (byte[] data);

    // data is native
    private void[] m_data;
}


