
; GIMP 3 batch resize:
; PNG files in input-dir -> JPEG files in output-dir
; Resize only if wider than max-width, preserving aspect ratio.

(define (batch-resize input-dir output-dir)
	(script-fu-use-v3)

	(let* ((filelist
		(file-glob 
			#:pattern (string-append input-dir "\\*.png")
			#:filename-encoding TRUE)
			)
		)
		
		(display filelist)
		(newline)						
			
		(while (not (null? filelist))
			(let* 
				((filename (car filelist))
					(image (gimp-file-load RUN-NONINTERACTIVE filename))	
					(parts (strbreakup filename "\\"))
					(filepart (car (reverse parts)))
					(nameparts (strbreakup filepart "."))
					(basename
						(unbreakupstr
							(reverse (cdr (reverse nameparts)))
						".")
					)

					(outfile
						(string-append output-dir "\\"
						basename ".png")
					)
				)

			;------- 
			;Creates round black icons to be used 
			
			
			(gimp-image-resize image 64 131 0 43)
			;------- 

			; Export
			(gimp-file-save RUN-NONINTERACTIVE image outfile)

			(gimp-image-delete image))

			(set! filelist (cdr filelist))
		)	
	)
)
      
; Run:
(batch-resize
  "C:\\ARCode\\DarcMods\\DarcCore\\UI\\Textures\\ExportedIcons"
  "C:\\ARCode\\DarcMods\\DarcCore\\UI\\Textures\\ExportedIcons\\done"
)
