#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <systemctrl.h>
#include <string.h>
#define MODULE_NAME "nipvp"
PSP_MODULE_INFO(MODULE_NAME, 0x1007, 1, 0);

#define EMULATOR_DEVCTL__IS_EMULATOR 0x03

#define REF(x) *((int*)(x))
#define REF_BYTE(x) *((char*)(x))
#define REF_HALF_WORD(x) *((short*)(x))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define J(addr) (0x8000000 | ((int)(addr) >> 2))

#define INIT_USER_MODULE_ADDR() int user_text_addr; get_from_reg(user_text_addr, at);
#define USER_ADDR(addr) ({INIT_USER_MODULE_ADDR(); (int)addr - (int)&__executable_start + (int)user_text_addr;})
#define J_USER(target) J(({INIT_USER_MODULE_ADDR(); ((int (*)(int, int, int))USER_ADDR(&make_trampoline))(1, user_text_addr, USER_ADDR(target));}))
#define USER_ALIAS_OF(name) (*((typeof(name)*)USER_ADDR(&name)))

#define REGION_ADDR 0x08901b6b

#define GAME_MODE_ADDR_JPN 0x8c05800
#define GAME_MODE_ADDR_USA 0x8c069c0

#define PLAYER_BASE_ADDR_JPN 0x8bD898C
#define PLAYER_BASE_ADDR_USA 0x8bD9B54

// #define PLAYER_BASE_ADDR2_JPN 0x08bdace4
// #define PLAYER_BASE_ADDR2_USA 0x08bdace4

#define MISSION_CODE_ADDR_JPN 0x8c05884
#define MISSION_CODE_ADDR_USA 0x8c06A44

#define PLAYER_INIT_HOOK_ADDR_JPN 0x08947a2c
#define PLAYER_INIT_HOOK_ADDR_USA 0x0894835c

#define PLAYER_INIT_HOOK_ADDR_2_JPN 0x0895bb28
#define PLAYER_INIT_HOOK_ADDR_2_USA 0x0895c458

#define PLAYER_1_SET_POS_HOOK_ADDR_JPN 0x08901470
#define PLAYER_1_SET_POS_HOOK_ADDR_USA 0x08901DA0

#define PLAYER_2_SET_POS_HOOK_ADDR_JPN 0x088FFA30
#define PLAYER_2_SET_POS_HOOK_ADDR_USA 0x08900360

#define LOAD_COORDINATE_HOOK_ADDR_JPN 0x08930a34
#define LOAD_COORDINATE_HOOK_ADDR_USA 0x08931364

#define SET_GAME_MODE_HOOK_ADDR_JPN 0x0883e770
#define SET_GAME_MODE_HOOK_ADDR_USA 0x0883f0a0

#define LOAD_TAG_MODE_HOOK_ADDR_JPN 0x089fa448
#define LOAD_TAG_MODE_HOOK_ADDR_USA 0x089fadc8

#define SET_ATTACK_VALUE_HOOK_ADDR_JPN 0x088963b8
#define SET_ATTACK_VALUE_HOOK_ADDR_USA 0x08896CE8

#define SET_GAME_MODE_HOOK_ADDR     (REF_BYTE(REGION_ADDR) == 0x0 ? SET_GAME_MODE_HOOK_ADDR_JPN     : SET_GAME_MODE_HOOK_ADDR_USA)
#define LOAD_TAG_MODE_HOOK_ADDR     (REF_BYTE(REGION_ADDR) == 0x0 ? LOAD_TAG_MODE_HOOK_ADDR_JPN     : LOAD_TAG_MODE_HOOK_ADDR_USA)

//#define IS_TAG_MODE (REF_BYTE(GAME_MODE_ADDR) == 0x02)
static int IS_TAG_MODE = 0;

extern char __executable_start;
extern char end;

typedef struct {
    int addr;
    int inst;
    int ori_inst;
} Patch;

#define FUNC_HOOK_NUM 9

#define MAX_INST_PATCHES 90
#define MAX_PATCHES      (FUNC_HOOK_NUM + MAX_INST_PATCHES + 1)

static Patch instruction_patch_set1[] = {
    // Remove enemies
    {0x08901238, 0x1000006B},
    // 2P Health Bar
    {0x08888A7C, 0x00000000},
    {0x0888D3AC, 0x00000000},
    {0x0888D544, 0x00000000},
    {0x08B95460, 0x0029009D},
    {0x08B95464, 0x009E0003},
    {0x08B95468, 0x00040029},
    {0x08B9546C, 0x0029009F},
    {0x08B95470, 0x00A50005},
    {0x08B95474, 0x00060029},
    {0x08B95478, 0x002900A5},
    {0x08B9547C, 0x00A30007},
    {0x08B95480, 0x00080029},
    {0x08B95484, 0x002900A3},
    {0x0886B460, 0xC60C09EC},
    {0x0886B464, 0xC60D12EC},
    {0x0886B468, 0x460C6300},
    {0x0886B46C, 0x460D6301},
    {0x0886B470, 0xE60C12EC},
    {0x0886B474, 0x10000005},
    {0x0886B478, 0x00000000},
    // 2P Awakening Bar (separate background below gauge; frame above)
    // Disable inherited cutscene face culling for mirrored HUD sprites.
    {0x0886A88C, 0x34040005},
    // COM Chakra: use original action costs and shared recovery; keep AI decisions.
    {0x0888F710, 0x00000000},
    {0x08893A04, 0x00000000},
    {0x0886EE7C, 0x34040060},
    {0x0886EE80, 0xACA44FC8},
    {0x0886EE84, 0xACA04FD4},
    {0x0886EE88, 0x3C04BF80},
    {0x0886EE8C, 0xAE04015C},
    {0x0886EE90, 0x3C044350},
    {0x0886EE94, 0x44846000},
    {0x0886EE98, 0x3C04C2CC},
    {0x0886EE9C, 0x44847000},
    {0x0886EEA0, 0xE60C0154},
    {0x0886EEA4, 0xE60E0158},
    // Hide misplaced 2P awakening prompts
    {0x0886BAD4, 0x2AC40082},
    {0x0886BAE8, 0x2AC40082},
    {0x0886BAFC, 0x2AC40082},
    {0x0886BB04, 0x2AC40082},
    {0x0886BB14, 0x2AC40082},
    {0x08B954FC, 0x01280049},
    // Disable Awakening Hit Immunity (only hit mode 1; preserve other modes)
    {0x08892D10, 0x92410826},
    {0x08892D14, 0x00410826},
    {0x08892D18, 0x0001100A},
    {0x08892D1C, 0xA2420827},
    // Disable Awakening Invincibility (keep non-awakening damage vetoes)
    {0x08896394, 0x92010826},
    {0x08896398, 0x0001100B},
    {0x0889639C, 0x144000AE},
    {0x088963A0, 0x86350006},
    {0x088963A4, 0x1AA0000E},
    // Time-based Awakening: disable attack/hit and item gauge gains
    {0x08892E38, 0x03E00008},
    {0x08892E3C, 0x00000000},
    {0x088816C0, 0x1000000A},
    // Awakening in Tag Mode (load CG resources for both player slots)
    {0x0886E8EC, 0x00000000},
    {0x0886EE74, 0x34050098}, // Keep 0x98: 0x9C moves the Lv anchors.
    {0x0888C750, 0x00000000},
    {0x0893622C, 0x00000000}, // CWCheat 16-bit zero; verified upper half is also zero.
    {0x0883BE00, 0x2413FFFE},
    {0x0883BE0C, 0x26640002},
    {0x0883BE18, 0x26640002},
    {0x0883BE38, 0x10400033},
    {0x0883BE3C, 0x26730001},
    {0x0883BEEC, 0xAC920008},
    {0x0883BEF0, 0x8E250000},
    {0x0883BEF4, 0x8CA60004},
    {0x0883BEF8, 0xAC850000},
    {0x0883BEFC, 0xAC860004},
    {0x0883BF00, 0xACC40000},
    {0x0883BF04, 0xACA40004},
    {0x0883BF08, 0x0660FFBF},
    {0x0883BF0C, 0x00000000},
    // Better Ending
    {0x08938E00, 0x00000000},
    // No more cache!
    {0x08939054, 0x00000000},
    {0x08938E10, 0x34040034},
    // No reviving
    {0x08938e78, 0x34040008},
    // Skip Opening
    {0x088FD920, 0x00000000},
    {0x089019f4, 0x10000004},
    {0x08901A1C, 0x34020000},
    {0x08939a4c, 0x00000000},
    // Remove Circles
    {0x08880808, 0x1000001A},
    // Ad-Hoc port
    {0x08814ecc, 0x3405030a},
    {0x08814b90, 0x3406030a},
    {0x08814ccc, 0x3416030a},
    // Ad-Hoc matching port
    {0x088117e8, 0x34060002},
    {0x088116f4, 0x34060002},
    // 1P Balance - load per-character Lv99/2 stats
    {0x08892864, 0x97A50054},
    {0x08892860, 0x97A40056},
    // 2P Balance - load per-character Lv99/2 stats
    {0x08892A04, 0x97A50114},
    {0x08892A00, 0x97A40116},
    // DEF Balance
    {0x08896258, 0x34020150},
    {}, // end
};

static Patch instruction_patch_set2[] = {
    // Remove enemies
    {0x08901B68,0x1000006B},
    // 2P Health Bar
    {0x088893AC, 0x00000000},
    {0x0888DCDC, 0x00000000},
    {0x0888DE74, 0x00000000},
    {0x08B96670, 0x0029009D},
    {0x08B96674, 0x009E0003},
    {0x08B96678, 0x00040029},
    {0x08B9667C, 0x0029009F},
    {0x08B96680, 0x00A50005},
    {0x08B96684, 0x00060029},
    {0x08B96688, 0x002900A5},
    {0x08B9668C, 0x00A30007},
    {0x08B96690, 0x00080029},
    {0x08B96694, 0x002900A3},
    {0x0886BD90, 0xC60C09EC},
    {0x0886BD94, 0xC60D12EC},
    {0x0886BD98, 0x460C6300},
    {0x0886BD9C, 0x460D6301},
    {0x0886BDA0, 0xE60C12EC},
    {0x0886BDA4, 0x10000005},
    {0x0886BDA8, 0x00000000},
    // 2P Awakening Bar (separate background below gauge; frame above)
    // Disable inherited cutscene face culling for mirrored HUD sprites.
    {0x0886B1BC, 0x34040005},
    // COM Chakra: use original action costs and shared recovery; keep AI decisions.
    {0x08890040, 0x00000000},
    {0x08894334, 0x00000000},
    {0x0886F7AC, 0x34040060},
    {0x0886F7B0, 0xACA44FC8},
    {0x0886F7B4, 0xACA04FD4},
    {0x0886F7B8, 0x3C04BF80},
    {0x0886F7BC, 0xAE04015C},
    {0x0886F7C0, 0x3C044350},
    {0x0886F7C4, 0x44846000},
    {0x0886F7C8, 0x3C04C2CC},
    {0x0886F7CC, 0x44847000},
    {0x0886F7D0, 0xE60C0154},
    {0x0886F7D4, 0xE60E0158},
    // Hide misplaced 2P awakening prompts
    {0x0886C404, 0x2AC40082},
    {0x0886C418, 0x2AC40082},
    {0x0886C42C, 0x2AC40082},
    {0x0886C434, 0x2AC40082},
    {0x0886C444, 0x2AC40082},
    {0x08B9670C, 0x01280049},
    // Disable Awakening Hit Immunity (only hit mode 1; preserve other modes)
    {0x08893640, 0x92410826},
    {0x08893644, 0x00410826},
    {0x08893648, 0x0001100A},
    {0x0889364C, 0xA2420827},
    // Disable Awakening Invincibility (keep non-awakening damage vetoes)
    {0x08896CC4, 0x92010826},
    {0x08896CC8, 0x0001100B},
    {0x08896CCC, 0x144000AE},
    {0x08896CD0, 0x86350006},
    {0x08896CD4, 0x1AA0000E},
    // Time-based Awakening: disable attack/hit and item gauge gains
    {0x08893768, 0x03E00008},
    {0x0889376C, 0x00000000},
    {0x08881FF0, 0x1000000A},
    // Awakening in Tag Mode (load CG resources for both player slots)
    {0x0886F21C, 0x00000000},
    {0x0886F7A4, 0x34050098}, // Keep 0x98: 0x9C moves the Lv anchors.
    {0x0888D080, 0x00000000},
    {0x08936B5C, 0x00000000}, // CWCheat 16-bit zero; verified upper half is also zero.
    {0x0883C730, 0x2413FFFE},
    {0x0883C73C, 0x26640002},
    {0x0883C748, 0x26640002},
    {0x0883C768, 0x10400033},
    {0x0883C76C, 0x26730001},
    {0x0883C81C, 0xAC920008},
    {0x0883C820, 0x8E250000},
    {0x0883C824, 0x8CA60004},
    {0x0883C828, 0xAC850000},
    {0x0883C82C, 0xAC860004},
    {0x0883C830, 0xACC40000},
    {0x0883C834, 0xACA40004},
    {0x0883C838, 0x0660FFBF},
    {0x0883C83C, 0x00000000},
    // Better Ending
    {0x08939730, 0x00000000},
    // No more cache!
    {0x08939984, 0x00000000},
    {0x08939740, 0x34040034},
    // No reviving
    {0x089397a8, 0x34040008},
    // Skip Opening
    {0x088FE250, 0x00000000},
    {0x08902324, 0x10000004},
    {0x0890234C, 0x34020000},
    {0x0893a37c, 0x00000000},
    // Remove Circles
    {0x08881138, 0x1000001A},
    // Ad-Hoc port
    {0x08814ef0, 0x3405030a},
    {0x08814bb4, 0x3406030a},
    {0x08814cf0, 0x3416030a},
    // Ad-Hoc matching port
    {0x0881180c, 0x34060002},
    {0x08811718, 0x34060002},
    // 1P Balance - load per-character Lv99/2 stats
    {0x08893194, 0x97A50054},
    {0x08893190, 0x97A40056},
    // 2P Balance - load per-character Lv99/2 stats
    {0x08893334, 0x97A50114},
    {0x08893330, 0x97A40116},
    // DEF Balance
    {0x08896B88, 0x34020150},
    {}, // end
};

static struct {
    Patch function_hook_patches[FUNC_HOOK_NUM];
    Patch instruction_patches[MAX_INST_PATCHES + 1];
} _patches;

void init_patch_buffer(void) {
    static int inited = 0;
    if (inited) return;
    inited = 1;
    Patch *src = (REF_BYTE(REGION_ADDR)==0
                  ? instruction_patch_set1
                  : instruction_patch_set2);

    Patch *dst = _patches.instruction_patches;
    int i;
    for (i = 0; i < MAX_INST_PATCHES && src[i].addr; i++) {
        dst[i].addr     = src[i].addr;
        dst[i].inst     = src[i].inst;
        dst[i].ori_inst = 0;
    }
    // sentinel
    dst[i].addr     = 0;
    dst[i].inst     = 0;
    dst[i].ori_inst = 0;
}

#define patches ((Patch*)USER_ADDR(&_patches))

#define IS_VALID_ADDR(x) ((int)(x) >= 0x08800000 && (int)(x) <= 0x0A000000)


typedef struct {
    int idx;
    float x;
    float z;
    float y;
    float unknown1;
    int orientation;
} Coordinate;

#define get_from_reg(var, reg) asm( \
    ".set noat\n" \
    "move %0,$"#reg \
    : "=r" (var) \
);

#define save_to_reg(reg, var) asm( \
    "move $"#reg",%0" \
    : \
    : "r" (var) \
);

#define save_to_stack(reg) asm( \
    "addiu $sp,$sp,-4\n" \
    "sw $"#reg",0x0($sp)" \
);

#define restore_from_stack(reg) asm( \
    "lw $"#reg",0x0($sp)\n" \
    "addiu $sp,$sp,4" \
);

#define return_to(addr) asm( \
    "move $ra,%0\n" \
    : \
    : "r" (addr) \
    : "v0" \
);

static int _counter = 0;
#define counter USER_ALIAS_OF(_counter)


void player_info_hook() {

    if (counter == 0) {
        asm volatile (
            "li   $a3, 0x2\n"
            "sw   $a3, 0xA4($s0)\n"
        );
    }
    counter++;
    if (counter >= 2)
        counter = 0;
    
    asm volatile("jr $ra\n");
}

// Awakening and battle restart rerun this hook without player_info_hook.
// Select the PvP faction from the controller identity, never shared call parity.
// Local 1P (controller 0) uses faction 2; the COM partner uses faction 1.
void player_info_hook_2() {
    asm volatile (
        "lw    $a3, 0x20($s0)\n"
        "sltiu $a3, $a3, 1\n"
        "addiu $a3, $a3, 1\n"
        "sw    $a3, 0x538($s0)\n"
        "sw    $a3, 0x8($s0)\n"
        "jr    $ra\n"
        "nop\n"
    );
}


// The normal combat update runs at 30 simulation ticks/second.
// Rate = (3 - 2 * HP/maxHP) full gauges per 90 seconds.
// Keep fractional progress outside game actor memory; slots never share it.
#define AWAKENING_FULL_HP_SECONDS 90
#define AWAKENING_TICKS_PER_SECOND 30
#define AWAKENING_RATE_SCALE 256U

typedef struct {
    int actor;
    int last_gauge;
    unsigned int remainder;
} AwakeningCharge;
static AwakeningCharge _awakening_charge[2];

int awakening_charge_hook(int actor) {
    AwakeningCharge *states = (AwakeningCharge *)USER_ADDR(&_awakening_charge);
    int original = REF_BYTE(REGION_ADDR) == 0 ? 0x08896938 : 0x08897268;
    int identity = REF(actor + 4);
    if (identity == 1 || identity == 2) {
        AwakeningCharge *state = &states[identity - 1];
        int gauge = REF(actor + 0x7C0);
        int maximum = REF(actor + 0x7D8);
        int hp = REF(actor + 0x7B8);
        int max_hp = REF(actor + 0x7D0);
        int action = REF(actor + 0x60);
        if (state->actor != actor || state->last_gauge != gauge) {
            state->actor = actor;
            state->remainder = 0;
        }
        if (REF_BYTE(actor + 0x826) || hp <= 0 || max_hp <= 0 ||
            maximum <= 0 || gauge >= maximum) {
            state->remainder = 0;
        } else if (action != 0x101D && action != 0x201E) {
            unsigned int ratio;
            unsigned int points;
            const unsigned int denominator = AWAKENING_FULL_HP_SECONDS *
                AWAKENING_TICKS_PER_SECOND * AWAKENING_RATE_SCALE;
            if (hp > max_hp) hp = max_hp;
            ratio = (unsigned int)hp * AWAKENING_RATE_SCALE / (unsigned int)max_hp;
            state->remainder += (unsigned int)maximum *
                (3U * AWAKENING_RATE_SCALE - 2U * ratio);
            points = state->remainder / denominator;
            state->remainder %= denominator;
            gauge += (int)points;
            if (gauge >= maximum) {
                gauge = maximum;
                state->remainder = 0;
            }
            REF(actor + 0x7C0) = gauge;
        }
        state->last_gauge = gauge;
    }
    // Keep the original awakening countdown and end-of-awakening result.
    return ((int (*)(int))original)(actor);
}

// Normalize only battle stat queries. Keep profile level and experience.
// Equipment stat additions are excluded so effective growth stats are Lv99/2.
// Stat block mapping is the same as the game's base-stat builder.
int level99_balance_hook(int *profile, int character, int form, unsigned char *out) {
    unsigned short growth[12];
    int jp = REF_BYTE(REGION_ADDR) == 0;
    int query = 0x08A620CC - jp * 0x85C;
    int growth_query = 0x08A61560 - jp * 0x85C;
    int result = ((int (*)(int *, int, int, unsigned char *))query)
        (profile, character, form, out);
    int i;
    ((void (*)(unsigned short *, int, int))growth_query)(growth, character, 99);
    for (i = 0; i < 10; ++i) {
        int stat = i + 1 - 2 * (i >= 5) + ((unsigned int)(i - 6) < 2) - (i >= 8) + (i == 9);
        unsigned short *base = (unsigned short *)(out + 0x2C + i * 2);
        unsigned short *effective = (unsigned short *)(out + 0x50 + i * 2);
        int value = growth[stat];
        // Match the original stat cap; index 2 has no 999 cap.
        if (i != 2 && value > 999) value = 999;
        value /= 2;
        *base = value;
        *effective = value;
    }
    return result;
}

void set_attack_value_hook() {

    int value;
    int multiplier;
    
    asm("lh %0, 0x6($s1)" : "=r"(value));
    asm("lh %0, 0x34($s1)" : "=r"(multiplier));

    if (value > 65) {
        value -= 20;
    }
    if (value > 8 && value < 20) {
        value = 6;
    }
    if (multiplier < 580){
        multiplier = 580;
    }
    if (multiplier > 670){
        if (value < 20) {
            multiplier = 580;
        }
        else{
            if (value < 65){
            multiplier += 50;
            }
        }
    }

    asm("move $s5, %0" :: "r"(value));
    asm("move $a0, %0" :: "r"(multiplier));
    if (REF_BYTE(REGION_ADDR) == 0x0)
        return_to(SET_ATTACK_VALUE_HOOK_ADDR_JPN + 4)
    else
        return_to(SET_ATTACK_VALUE_HOOK_ADDR_USA + 4)
}

int _p1_pos_idx;
int _p2_pos_idx;
#define p1_pos_idx USER_ALIAS_OF(_p1_pos_idx)
#define p2_pos_idx USER_ALIAS_OF(_p2_pos_idx)

void set_p1_pos_hook() {
    save_to_stack(a0);
    save_to_stack(a2);
    asm(
        "lhu %0,0x8($s6)\n"
        : "=r" (p1_pos_idx)
    );
    p2_pos_idx = p1_pos_idx + 1;
    int mission;
    if (REF_BYTE(REGION_ADDR) == 0x0)
        mission = REF(MISSION_CODE_ADDR_JPN);
    else
        mission = REF(MISSION_CODE_ADDR_USA);
    if (mission == 0x9d || mission == 0x9e || mission == 0xa8) {
        p2_pos_idx = p1_pos_idx - 1;
    }
    save_to_reg(a1, p1_pos_idx);
    restore_from_stack(a2);
    restore_from_stack(a0);
    if (REF_BYTE(REGION_ADDR) == 0x0)
        return_to(PLAYER_1_SET_POS_HOOK_ADDR_JPN + 8)
    else
        return_to(PLAYER_1_SET_POS_HOOK_ADDR_USA + 8)
}

void set_p2_pos_hook() {
    save_to_stack(a0);
    save_to_reg(a1, p2_pos_idx);
    restore_from_stack(a0);
    if (REF_BYTE(REGION_ADDR) == 0x0)
        return_to(PLAYER_2_SET_POS_HOOK_ADDR_JPN + 8)
    else
        return_to(PLAYER_2_SET_POS_HOOK_ADDR_USA + 8)
}

static float _offset = 100.;
#define offset USER_ALIAS_OF(_offset)

void load_coordinate_hook() {
    save_to_stack(a0);
    Coordinate *coordinate_list = 0;
    get_from_reg(coordinate_list, v0);
    if (!coordinate_list || coordinate_list[0].idx != p1_pos_idx) {  // not a coordinate list
        goto exit;
    }

    Coordinate *p1_coordinate = coordinate_list;
    Coordinate *p2_coordinate = coordinate_list - p1_pos_idx + p2_pos_idx;

    int orientation = p1_coordinate->orientation * 180 / 2048;
    if (orientation < 0) {
        orientation += 360;
    }
    if (orientation < 45) {
        p2_coordinate->x = p1_coordinate->x;
        p2_coordinate->y = p1_coordinate->y + offset;
    } else if (orientation < 135) {
        p2_coordinate->x = p1_coordinate->x + offset;
        p2_coordinate->y = p1_coordinate->y;
    } else if (orientation < 225) {
        p2_coordinate->x = p1_coordinate->x;
        p2_coordinate->y = p1_coordinate->y - offset;
    } else if (orientation < 315) {
        p2_coordinate->x = p1_coordinate->x - offset;
        p2_coordinate->y = p1_coordinate->y;
    }
    p2_coordinate->orientation = p1_coordinate->orientation + 2048;

    exit:
    restore_from_stack(a0);
    asm("lui $a1,0x3f80");  // original instruction

    if (REF_BYTE(REGION_ADDR) == 0x0)
        return_to(LOAD_COORDINATE_HOOK_ADDR_JPN + 8)
    else
        return_to(LOAD_COORDINATE_HOOK_ADDR_USA + 8)
}

int _trampolines [FUNC_HOOK_NUM * 3] = {};
int _trampolines_used_count = 0;
#define trampolines USER_ALIAS_OF(_trampolines)
#define trampolines_used_count USER_ALIAS_OF(_trampolines_used_count)

int make_trampoline(int user_mode, int user_text_addr, int target) {
    int tramp_addr;
    if (user_mode) {
        tramp_addr = (int)(trampolines + trampolines_used_count++ * 3);
    } else {
        SceUID block_id = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, "", PSP_SMEM_High, 12, NULL);
        tramp_addr = (int)sceKernelGetBlockHeadAddr(block_id);
    }
    REF(tramp_addr) = 0x3c010000 | (user_text_addr >> 16); // lui at,addr_h
    REF(tramp_addr + 4) = J(target);
    REF(tramp_addr + 8) = 0x34210000 | (user_text_addr & 0x0000ffff); // ori at,at,addr_l
    return tramp_addr;
}


inline static void patch_instruction(int addr, int inst) {
    REF(addr) = inst;
    asm(
        "cache 0x1a,0x0(%0)\n"
        "cache 8,0x0(%0)"
        :
        : "r" (addr)
    );
}

static int _is_patched = 0;
#define is_patched USER_ALIAS_OF(_is_patched)

void _start(int ignore_mode) {
    IS_TAG_MODE = REF_BYTE(REF_BYTE(REGION_ADDR) == 0x0 ? GAME_MODE_ADDR_JPN : GAME_MODE_ADDR_USA) == 0x02;
    if (!is_patched && (IS_TAG_MODE || ignore_mode)) {
        is_patched = 1;
        int hook_i = 0;
        trampolines_used_count = 0;
        
        if (REF_BYTE(REGION_ADDR) == 0x0) {
            patches[hook_i++] = (Patch) {PLAYER_INIT_HOOK_ADDR_JPN, J_USER(&player_info_hook)};
            patches[hook_i++] = (Patch) {PLAYER_INIT_HOOK_ADDR_2_JPN, J_USER(&player_info_hook_2)};
            patches[hook_i++] = (Patch) {LOAD_COORDINATE_HOOK_ADDR_JPN, J_USER(&load_coordinate_hook)};
            patches[hook_i++] = (Patch) {PLAYER_1_SET_POS_HOOK_ADDR_JPN, J_USER(&set_p1_pos_hook)};
            patches[hook_i++] = (Patch) {PLAYER_2_SET_POS_HOOK_ADDR_JPN, J_USER(&set_p2_pos_hook)};
            patches[hook_i++] = (Patch) {SET_ATTACK_VALUE_HOOK_ADDR_JPN, J_USER(&set_attack_value_hook)};
            patches[hook_i++] = (Patch) {0x089570A8, J_USER(&awakening_charge_hook) | 0x04000000};
            patches[hook_i++] = (Patch) {0x08892858, J_USER(&level99_balance_hook) | 0x04000000};
            patches[hook_i++] = (Patch) {0x088929F8, J_USER(&level99_balance_hook) | 0x04000000};
        }
        else{
            patches[hook_i++] = (Patch) {PLAYER_INIT_HOOK_ADDR_USA, J_USER(&player_info_hook)};
            patches[hook_i++] = (Patch) {PLAYER_INIT_HOOK_ADDR_2_USA, J_USER(&player_info_hook_2)};
            patches[hook_i++] = (Patch) {LOAD_COORDINATE_HOOK_ADDR_USA, J_USER(&load_coordinate_hook)};
            patches[hook_i++] = (Patch) {PLAYER_1_SET_POS_HOOK_ADDR_USA, J_USER(&set_p1_pos_hook)};
            patches[hook_i++] = (Patch) {PLAYER_2_SET_POS_HOOK_ADDR_USA, J_USER(&set_p2_pos_hook)};
            patches[hook_i++] = (Patch) {SET_ATTACK_VALUE_HOOK_ADDR_USA, J_USER(&set_attack_value_hook)};
            patches[hook_i++] = (Patch) {0x089579D8, J_USER(&awakening_charge_hook) | 0x04000000};
            patches[hook_i++] = (Patch) {0x08893188, J_USER(&level99_balance_hook) | 0x04000000};
            patches[hook_i++] = (Patch) {0x08893328, J_USER(&level99_balance_hook) | 0x04000000};
        }
        
        for (Patch *patch = patches; patch->addr; patch++) {
            patch->ori_inst = REF(patch->addr);
            patch_instruction(patch->addr, patch->inst);
        }
    } else if (is_patched && !IS_TAG_MODE && !ignore_mode) {
        is_patched = 0;

        for (Patch *patch = patches; patch->addr; patch++) {
            patch_instruction(patch->addr, patch->ori_inst);
        }
    }
}


void mode_changed_hook() {
    ((void (*)(int))(USER_ADDR(_start)))(0);
}

void tag_mode_entered_hook() {
    ((void (*)(int))(USER_ADDR(_start)))(1);
}

void init(int user_text_addr) {
    patch_instruction(SET_GAME_MODE_HOOK_ADDR, J(make_trampoline(0, user_text_addr, (char*)&mode_changed_hook - &__executable_start + user_text_addr)));
    patch_instruction(LOAD_TAG_MODE_HOOK_ADDR, J(make_trampoline(0, user_text_addr, (char*)&tag_mode_entered_hook - &__executable_start + user_text_addr)));
}

void load_module_to_user_space() {
    int elf_size = &end - &__executable_start;
    SceUID block_id = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, "", PSP_SMEM_High, elf_size, NULL);
    int user_text_addr = (u32)sceKernelGetBlockHeadAddr(block_id);
    memcpy((void*)user_text_addr, &__executable_start, elf_size);
    init(user_text_addr);
}

static STMOD_HANDLER previous;
int OnModuleStart(SceModule2 *mod) {
	if (strcmp(mod->modname, "Model") == 0) {
        init_patch_buffer();
	    load_module_to_user_space();
	}
	if (!previous)
		return 0;
	return previous(mod);
}

int module_start(SceSize args, void* argp) {
    int is_emulator = 0;
    sceIoDevctl("kemulator:", EMULATOR_DEVCTL__IS_EMULATOR, NULL, 0, &is_emulator, 4);

    if (is_emulator) {
        init_patch_buffer();
        load_module_to_user_space();
    } else {
        previous = sctrlHENSetStartModuleHandler(OnModuleStart);
    }

    return 0;
}
