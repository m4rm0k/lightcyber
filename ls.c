/* Lightcyber - 64k Intro by Team210 at Evoke 2k19
 * Copyright (C) 2019 Alexander Kraus <nr4@z10.info>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define DEBUG // Shader debug i/o
#define MIDI // APC40 mkII controls

const char *demoname = "Lightcyber/Team210";
unsigned int muted = 0.;

int _fltused = 0;

#include "common.h"

#ifdef MIDI

void select_button(int index)
{
    for(int i=0; i<40; ++i)
    {
        DWORD out_msg = 0x8 << 4 | i << 8 | 0 << 16;
        midiOutShortMsg(hMidiOut, out_msg);
    }

    if(index < 40)
    {
        override_index = index+1;
        scene_override = 1;
    }
    
    DWORD out_msg = 0x9 << 4 | index << 8 | 57 << 16;
    midiOutShortMsg(hMidiOut, out_msg);
}

#define NOTE_OFF 0x8
#define NOTE_ON 0x9
#define CONTROL_CHANGE 0xB

// #define APC_TOPDIAL 0x3
// #define APC_RIGHTDIAL 0x1
// #define APC_FADER
// #define APC_BUTTONMATRIX 0x2
void CALLBACK MidiInProc_apc40mk2(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if(wMsg == MIM_DATA)
    {
        BYTE b1 = (dwParam1 >> 24) & 0xFF,
            b2 = (dwParam1 >> 16) & 0xFF,
            b3 = (dwParam1 >> 8) & 0xFF,
            b4 = dwParam1 & 0xFF;
        BYTE b3lo = b3 & 0xF,
            b3hi = (b3 >> 4) & 0xF,
            b4lo = b4 & 0xF,
            b4hi = (b4 >> 4) & 0xF;
        
        BYTE channel = b4lo,
            button = b3;
            
        if(b4hi == NOTE_ON)
        {
            waveOutReset(hWaveOut);
            select_button(button);
            
            if(button == 0)
            {
                header.lpData = smusic1;
                header.dwBufferLength = 4 * music1_size;
                waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutRestart(hWaveOut);
            }
            else if(button == 1)
            {
                int delta = 49.655 * (double)sample_rate;
                header.lpData = smusic1+delta;
                header.dwBufferLength = 4 * (music1_size-delta);
                waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutRestart(hWaveOut);
            }
            else if(button == 2)
            {
                int delta = 82.76 * (double)sample_rate;
                header.lpData = smusic1+delta;
                header.dwBufferLength = 4 * (music1_size-delta);
                waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutRestart(hWaveOut);
            }
            else if(button == 3)
            {
                int delta = 99.31 * (double)sample_rate;
                header.lpData = smusic1+delta;
                header.dwBufferLength = 4 * (music1_size-delta);
                waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutRestart(hWaveOut);
            }
        }
        else if(b4hi == NOTE_OFF)
        {
            select_button(button);
        }
        else if(b4hi == CONTROL_CHANGE)// Channel select
        {
            printf("Control change in channel %d\n", channel);
        }

        printf("wMsg=MIM_DATA, dwParam1=%08x, byte=%02x %02x h_%01x l_%01x %02x, dwParam2=%08x\n", dwParam1, b1, b2, b3hi, b3lo, b4, dwParam2);
    }
    
	return;
}
#endif

void load_demo()
{
	// Load loading bar shader
	printf("++++ Creating Loading bar.\n");
	int load_size = strlen(load_frag);
	load_handle = glCreateShader(GL_FRAGMENT_SHADER);
	load_program = glCreateProgram();
	glShaderSource(load_handle, 1, (GLchar **)&load_frag, &load_size);
	glCompileShader(load_handle);
	printf("---> Load shader:\n");
#ifdef DEBUG
	debug(load_handle);
#endif
	glAttachShader(load_program, load_handle);
	glLinkProgram(load_program);
	printf("---> Load Program:\n");
#ifdef DEBUG
	debugp(load_program);
#endif
	glUseProgram(load_program);
	load_progress_location = glGetUniformLocation(load_program, LOAD_VAR_IPROGRESS);
	load_time_location = glGetUniformLocation(load_program, LOAD_VAR_ITIME);
	load_resolution_location = glGetUniformLocation(load_program, LOAD_VAR_IRESOLUTION);
	printf("++++ Loading bar created.\n");

	// Load post processing shader
	printf("++++ Creating Post Shader.\n");
	int post_size = strlen(post_frag);
	post_handle = glCreateShader(GL_FRAGMENT_SHADER);
	post_program = glCreateProgram();
	glShaderSource(post_handle, 1, (GLchar **)&post_frag, &post_size);
	glCompileShader(post_handle);
	printf("---> Post shader:\n");
#ifdef DEBUG
	debug(post_handle);
#endif
	glAttachShader(post_program, post_handle);
	glLinkProgram(post_program);
	printf("---> Post Program:\n");
#ifdef DEBUG
	debugp(post_program);
#endif
	glUseProgram(post_program);
	post_channel0_location = glGetUniformLocation(post_program, POST_VAR_ICHANNEL0);
	post_fsaa_location = glGetUniformLocation(post_program, POST_VAR_IFSAA);
	post_resolution_location = glGetUniformLocation(post_program, POST_VAR_IRESOLUTION);
    post_time_location = glGetUniformLocation(post_program, POST_VAR_ITIME);
	printf("++++ Post shader created.\n");

    // Load ui shader
	printf("++++ Creating Ui Shader.\n");
	int ui_size = strlen(ui_frag);
	ui_handle = glCreateShader(GL_FRAGMENT_SHADER);
	ui_program = glCreateProgram();
	glShaderSource(ui_handle, 1, (GLchar **)&ui_frag, &ui_size);
	glCompileShader(ui_handle);
	printf("---> Ui shader:\n");
#ifdef DEBUG
	debug(ui_handle);
#endif
	glAttachShader(ui_program, ui_handle);
	glLinkProgram(ui_program);
	printf("---> Ui Program:\n");
#ifdef DEBUG
	debugp(ui_program);
#endif
	glUseProgram(ui_program);
	ui_channel0_location = glGetUniformLocation(ui_program, UI_VAR_ICHANNEL0);
	ui_resolution_location = glGetUniformLocation(ui_program, UI_VAR_IRESOLUTION);
    ui_time_location = glGetUniformLocation(ui_program, UI_VAR_ITIME);
    ui_maxtime_location = glGetUniformLocation(ui_program, UI_VAR_IMAXTIME);
    ui_mouse_location = glGetUniformLocation(ui_program, UI_VAR_IMOUSE);
    ui_playing_location = glGetUniformLocation(ui_program, UI_VAR_IPLAYING);
	printf("++++ Ui shader created.\n");
    
	// Create framebuffer for rendering first pass to
	glGenFramebuffers(1, &first_pass_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, first_pass_framebuffer);
	glGenTextures(1, &first_pass_texture);
	glBindTexture(GL_TEXTURE_2D, first_pass_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, first_pass_texture, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// Generate music framebuffer
	// Initialize sequence texture
	printf("sequence texture width is: %d\n", sequence_texture_size); // TODO: remove
	glGenTextures(1, &sequence_texture_handle);
	glBindTexture(GL_TEXTURE_2D, sequence_texture_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sequence_texture_size, sequence_texture_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, sequence_texture);

	glGenFramebuffers(1, &snd_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, snd_framebuffer);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	unsigned int snd_texture;
	glGenTextures(1, &snd_texture);
	glBindTexture(GL_TEXTURE_2D, snd_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texs, texs, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, snd_texture, 0);

	// Music allocs
	nblocks1 = sample_rate * duration1 / block_size + 1;
	music1_size = nblocks1 * block_size;
	smusic1 = (float*)malloc(4 * music1_size);
	short *dest = (short*)smusic1;
	for (int i = 0; i < 2 * music1_size; ++i)
		dest[i] = 0;

	updateBar();

	// Load music shader
	LoadMusicThread(0);
	updateBar();

	// Load Logo 210 shader
//     LoadDecayingfactoryThread(0);
//     updateBar();
//     SwapBuffers(hdc);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	LoadSymbols();
	LoadPrograms();

	LoadTextThread(0);
	updateBar();

	for (int music_block = 0; music_block < nblocks1; ++music_block)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, snd_framebuffer);
		glUseProgram(sfx_program);

		printf("Rendering SFX block %d/%d -> %le\n", music_block, nblocks1, .5*(float)music_block / (float)nblocks1);
		double tstart = (double)(music_block*block_size);

		glViewport(0, 0, texs, texs);

		glUniform1f(sfx_volumelocation, 1.);
		glUniform1f(sfx_samplerate_location, (float)sample_rate);
		glUniform1f(sfx_blockoffset_location, (float)tstart);
		glUniform1i(sfx_texs_location, texs);
		glUniform1i(sfx_sequence_texture_location, 0);
		glUniform1f(sfx_sequence_width_location, sequence_texture_size);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sequence_texture_handle);

		quad();

		glReadPixels(0, 0, texs, texs, GL_RGBA, GL_UNSIGNED_BYTE, smusic1 + music_block * block_size);
		glFlush();

		unsigned short *buf = (unsigned short*)smusic1;
		short *dest = (short*)smusic1;
		if (!muted)
			for (int j = 2 * music_block*block_size; j < 2 * (music_block + 1)*block_size; ++j)
				dest[j] = (buf[j] - (1 << 15));
		else
			for (int j = 2 * music_block*block_size; j < 2 * (music_block + 1)*block_size; ++j)
				dest[j] = 0.;

        progress += .5/nblocks1;
		updateBar();
	}

	glUseProgram(0);

	initialize_sound();
    
#ifdef MIDI
    UINT nMidiDeviceNum;
    MIDIINCAPS caps;
    
	nMidiDeviceNum = midiInGetNumDevs();
	if(nMidiDeviceNum == 0) 
    {
        printf("No MIDI input devices connected.\n");
    }
    else
    {
        printf("Available MIDI devices:\n");
        for (unsigned int i = 0; i < nMidiDeviceNum; ++i) 
        {
            midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
            printf("->%d: %s ", i, caps.szPname);
            
            if(!strcmp("APC40 mkII", caps.szPname))
            {
                HMIDIIN hMidiDevice;
                MMRESULT rv = midiInOpen(&hMidiDevice, i, (DWORD)(void*)MidiInProc_apc40mk2, 0, CALLBACK_FUNCTION);
                midiInStart(hMidiDevice);
                
                printf(" >> opened.\n");
            }
            else
            {
                printf("(Unsupported MIDI controller).\n");
            }
        }
    }
    
    MIDIOUTCAPS ocaps;
    nMidiDeviceNum = midiOutGetNumDevs();

    if(nMidiDeviceNum == 0) 
    {
        printf("No MIDI output devices connected.\n");
    }
    else
    {
        printf("Available MIDI devices:\n");
        for (unsigned int i = 0; i < nMidiDeviceNum; ++i) 
        {
            midiOutGetDevCaps(i, &ocaps, sizeof(MIDIOUTCAPS));
            printf("->%d: %s ", i, ocaps.szPname);
            
            if(!strcmp("APC40 mkII", ocaps.szPname))
            {
                MMRESULT rv = midiOutOpen (&hMidiOut, i, 0, 0, CALLBACK_NULL);
            }
            else
            {
                printf("(Unsupported MIDI controller).\n");
            }
        }
    }
    
#endif
}

unsigned long __stdcall LoadMusicThread( void *lpParam)
{
    int sfx_size = strlen(sfx_frag);
    sfx_handle = glCreateShader(GL_FRAGMENT_SHADER);
    sfx_program = glCreateProgram();
    glShaderSource(sfx_handle, 1, (GLchar **)&sfx_frag, &sfx_size);
    glCompileShader(sfx_handle);
    printf("---> SFX shader:\n");
#ifdef DEBUG
    debug(sfx_handle);
#endif
    glAttachShader(sfx_program, sfx_handle);
    glLinkProgram(sfx_program);
    printf("---> SFX program:\n");
#ifdef DEBUG
    debugp(sfx_program);
#endif
    glUseProgram(sfx_program);
    sfx_samplerate_location = glGetUniformLocation(sfx_program, SFX_VAR_ISAMPLERATE);
    sfx_blockoffset_location = glGetUniformLocation(sfx_program, SFX_VAR_IBLOCKOFFSET);
    sfx_volumelocation = glGetUniformLocation(sfx_program, SFX_VAR_IVOLUME);
    sfx_texs_location = glGetUniformLocation(sfx_program, SFX_VAR_ITEXSIZE);
    sfx_sequence_texture_location = glGetUniformLocation(sfx_program, SFX_VAR_ISEQUENCE);
    sfx_sequence_width_location = glGetUniformLocation(sfx_program, SFX_VAR_ISEQUENCEWIDTH);
    printf("++++ SFX shader created.\n");
    
    music_loading = 1;
    progress += .1/NSHADERS; 
    
    return 0;
}

unsigned long __stdcall LoadLogo210Thread( void * lpParam)
{
    int logo210_size = strlen(logo210_frag);
    logo210_handle = glCreateShader(GL_FRAGMENT_SHADER);
    logo210_program = glCreateProgram();
    glShaderSource(logo210_handle, 1, (GLchar **)&logo210_frag, &logo210_size);
    glCompileShader(logo210_handle);
    printf("---> Logo 210 shader:\n");
#ifdef DEBUG
    debug(logo210_handle);
#endif
    glAttachShader(logo210_program, logo210_handle);
    glLinkProgram(logo210_program);
    printf("---> Logo 210 program:\n");
#ifdef DEBUG
    debugp(logo210_program);
#endif
    glUseProgram(logo210_program);
    logo210_time_location =  glGetUniformLocation(logo210_program, LOGO210_VAR_ITIME);
    logo210_resolution_location = glGetUniformLocation(logo210_program, LOGO210_VAR_IRESOLUTION);
    printf("++++ Logo 210 shader created.\n");
    
    progress += .1/NSHADERS;
    
    return 0;
}

unsigned long __stdcall LoadTextThread(void * lpParam)
{
    int text_size = strlen(text_frag);
    text_handle = glCreateShader(GL_FRAGMENT_SHADER);
    text_program = glCreateProgram();
    glShaderSource(text_handle, 1, (GLchar **)&text_frag, &text_size);
    glCompileShader(text_handle);
    printf("---> Text shader:\n");
#ifdef DEBUG
    debug(text_handle);
#endif
    glAttachShader(text_program, text_handle);
    glLinkProgram(text_program);
    printf("---> Text program:\n");
#ifdef DEBUG
    debugp(text_program);
#endif
    glUseProgram(text_program);
    text_time_location =  glGetUniformLocation(text_program, TEXT_VAR_ITIME);
    text_resolution_location = glGetUniformLocation(text_program, TEXT_VAR_IRESOLUTION);
    text_font_location= glGetUniformLocation(text_program, TEXT_VAR_IFONT);
    text_font_width_location = glGetUniformLocation(text_program, TEXT_VAR_IFONTWIDTH);
    text_channel0_location = glGetUniformLocation(text_program, TEXT_VAR_ICHANNEL0);
    printf("++++ Text shader created.\n");
    
    // Initialize font texture
    printf("font texture width is: %d\n", font_texture_size); // TODO: remove
    glGenTextures(1, &font_texture_handle);
    glBindTexture(GL_TEXTURE_2D, font_texture_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font_texture_size, font_texture_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, font_texture);
    
    progress += .1/NSHADERS;
    
    return 0;
}

void quad()
{
    glBegin(GL_QUADS);
    glVertex3f(-1,-1,0);
    glVertex3f(-1,1,0);
    glVertex3f(1,1,0);
    glVertex3f(1,-1,0);
    glEnd();
    glFlush();
}

void updateBar()
{
    glBindFramebuffer(GL_FRAMEBUFFER, first_pass_framebuffer);
    
    // Render first pass
    glViewport(0,0,w,h);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(load_program);
    glUniform2f(load_resolution_location, w, h);
    
    glUniform1f(load_progress_location, progress);
    
    quad();
    
    // Render second pass (Post processing) to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0,0,w,h);
    
    glUseProgram(post_program);
    glUniform2f(post_resolution_location, w, h);
    glUniform1f(post_fsaa_location, fsaa);
    glUniform1i(post_channel0_location, 0);
    glUniform1f(post_time_location, 0.);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, first_pass_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    
    quad();
    
    glBindTexture(GL_TEXTURE_2D, 0);

	flip_buffers();
}

// Pure opengl drawing code, essentially cross-platform
void draw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, first_pass_framebuffer);
    
    float t = t_now;
    if(t > t_end)
        ExitProcess(0);
    
    if(scene_override)
    {
        if(override_index == 1)
        {
            t = t_now;
        }
        else if(override_index == 2)
        {
            t = t_now + 49.655;
        }
        else if(override_index == 3)
        {
            t = t_now + 82.76;
        }
        else if(override_index == 4)
        {
            t = t_now + 99.31;
        }
    }
    
    if(t < 49.655)
    {
        glUseProgram(graffiti_program);
        glUniform1f(graffiti_iTime_location, t);
        glUniform2f(graffiti_iResolution_location, w, h);
        
#ifdef MIDI
        if(override_index == 0)
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 82.76)
    {
        glUseProgram(groundboxes_program);
        glUniform1f(groundboxes_iTime_location, t-49.655);
        glUniform2f(groundboxes_iResolution_location, w, h);
        
#ifdef MIDI
        if(override_index == 1) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 99.31)
    {
        glUseProgram(voronoidesign_program);
        glUniform1f(voronoidesign_iTime_location, t-82.76);
        glUniform2f(voronoidesign_iResolution_location, w, h);
      
#ifdef MIDI
        if(override_index == 2) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < t_end)
    {
        glUseProgram(bloodcells_program);
        glUniform1f(bloodcells_iTime_location, t-99.31);
        glUniform2f(bloodcells_iResolution_location, w, h);
      
#ifdef MIDI
        if(override_index == 3) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else ExitProcess(0);
    
    quad();

    // Render post processing to buffer
    glUseProgram(post_program);
    glUniform2f(post_resolution_location, w, h);
    glUniform1f(post_fsaa_location, fsaa);
    glUniform1i(post_channel0_location, 0);
    glUniform1f(post_time_location, t);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, first_pass_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    quad();
    
    // Render text to first pass framebuffer
    glUseProgram(text_program);
    glUniform2f(text_resolution_location, w, h);
    glUniform1f(text_font_width_location, font_texture_size);
    glUniform1f(text_time_location, t);
    glUniform1i(text_channel0_location, 0);
    glUniform1i(text_font_location, 1);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, first_pass_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, font_texture_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font_texture_size, font_texture_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    
    quad();
    
    // render ui to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glUseProgram(ui_program);
    glUniform1f(ui_time_location, t);
    glUniform1f(ui_maxtime_location, t_end);
    glUniform2f(ui_mouse_location, mx, h-my);
    glUniform2f(ui_resolution_location, w, h);
    glUniform1f(ui_playing_location, paused?0.:1.);
    glUniform1i(ui_channel0_location, 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, first_pass_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    quad();
    
    glBindTexture(GL_TEXTURE_2D, 0);
}



