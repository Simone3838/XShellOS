;
; 		NanoShell Operating System
;      (C) 2021-2022 iProgramInCpp
; 
;       Task Scheduling ASM Module
;
BITS 32

;section .data
;global e_TaskStack
;e_TaskStack res 1024

section .text

extern KeSwitchTask
extern KeOnEnterInterrupt
extern KeOnExitInterrupt

global IrqTaskA
global IrqTaskB

IrqTaskA:
	cli
	; Preserve basic registers
	push esp
	push ebp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push eax
	
	; Preserve page table
	mov eax, cr3
	push eax
	
	; Preserve segment registers
	mov eax, ds
	push eax
	mov eax, es
	push eax
	mov eax, fs
	push eax
	mov eax, gs
	push eax
	mov eax, ss
	push eax
	
	; acknowledge the interrupt
	mov al, 0x20
	mov dx, 0x20
	out dx, al
	mov dx, 0xA0
	out dx, al
	
	push esp
	
	; Disable the direction flag, mandatory for C 32-bit SysV ABI if
	; code relies on repeat instructions.
	; The direction flag will be restored to the task, since it's part
	; of the EFLAGS, and it's already saved.
	cld
	
	; push a 1, because this came from the PIT
	push 1
	
	call KeOnEnterInterrupt
	; call the re-schedule function
	call KeSwitchTask
	
	call KeOnExitInterrupt
	
	add esp, 8 ; get rid of what we had on the stack
	
	; Restore the seg registers
	pop eax
	mov ss, eax
	pop eax
	mov gs, eax
	pop eax
	mov fs, eax
	pop eax
	mov es, eax
	pop eax
	mov ds, eax
	
	; Restore page table
	pop eax
	mov cr3, eax
	
	; Restore working registers
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	pop esp
	
	; There is actually no need to call "sti", iretd 
	; reloads the EFLAGS, which automatically does that.
	iretd

IrqTaskB:
	cli
	; Preserve basic registers
	push esp
	push ebp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push eax
	
	; Preserve page table
	mov eax, cr3
	push eax
	
	; Preserve segment registers
	mov eax, ds
	push eax
	mov eax, es
	push eax
	mov eax, fs
	push eax
	mov eax, gs
	push eax
	mov eax, ss
	push eax
	
	; this came from software, no need to acknowledge anything :)
	
	push esp
	
	; Disable the direction flag, mandatory for C 32-bit SysV ABI if
	; code relies on repeat instructions.
	; The direction flag will be restored to the task, since it's part
	; of the EFLAGS, and it's already saved.
	cld
	
	; push a 0, because this came from software, rather than the PIT itself.
	push 0
	; call the re-schedule function
	call KeOnEnterInterrupt
	call KeSwitchTask
	call KeOnExitInterrupt
	add esp, 8 ; get rid of what we had on the stack
	
	; Restore the seg registers
	pop eax
	mov ss, eax
	pop eax
	mov gs, eax
	pop eax
	mov fs, eax
	pop eax
	mov es, eax
	pop eax
	mov ds, eax
	
	; Restore page table
	pop eax
	mov cr3, eax
	
	; Restore working registers
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	pop esp
	
	; There is actually no need to call "sti", iretd 
	; reloads the EFLAGS, which automatically does that.
	iretd
	
extern g_saveStateToRestore1
global KeStartedNewTask
global KeStartedNewKernelTask
KeStartedNewTask:
KeStartedNewKernelTask:
	; Get the save state pointer to restore
	mov esp, [g_saveStateToRestore1]
	
	; Restore the seg registers
	pop eax
	mov ss, eax
	pop eax
	mov gs, eax
	pop eax
	mov fs, eax
	pop eax
	mov es, eax
	pop eax
	mov ds, eax
	
	; Restore page table
	pop eax
	mov cr3, eax
	
	; Restore working registers
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	pop esp
	
	iretd

extern KeTaskStartupC
global KeTaskStartup
KeTaskStartup:
	; Sets up the task, calls its function, and then
	; kills it.  The task's pointer is in eax.
	
	push eax
	call KeTaskStartupC
	
	; freeze if we got to this point!?
.halt:
	hlt
	jmp .halt