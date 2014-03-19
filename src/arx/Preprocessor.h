#ifndef __ARX_PREPROCESSOR_H__
#define __ARX_PREPROCESSOR_H__

#include "config.h"

/**
 * @file
 *
 * This is ARX Preprocessor library header file. <br>
 * The macros defined here are used for high-level preprocessor metaprogramming. <br>
 * This library was inspired by BOOST PP library (and uses parts of it). <p>
 *
 * Please note that since preprocessor implementation varies from compiler to compiler, this library may not work correctly with unsupported compilers. <br>
 * Currently supported compilers:
 * <ul>
 * <li> <tt>MSVC 8.0</tt>
 * <li> <tt>GCC 3.4+</tt>
 * </ul>
 */

#ifdef ARX_USE_BOOST
#  include <boost/config.hpp>
#  define ARX_STRINGIZE BOOST_STRINGIZE

#  include <boost/preprocessor/cat.hpp>
#  define ARX_JOIN BOOST_PP_CAT

#  include <boost/preprocessor/logical.hpp>
#  define ARX_BOOL BOOST_PP_BOOL
#  define ARX_BITAND BOOST_PP_BITAND
#  define ARX_BITOR BOOST_PP_BITOR
#  define ARX_BITNOT BOOST_PP_COMPL
#  define ARX_AND BOOST_PP_AND
#  define ARX_OR BOOST_PP_OR
#  define ARX_NOT BOOST_PP_NOT

#  include <boost/preprocessor/control.hpp>
#  define ARX_IF BOOST_PP_IF
#  define ARX_IIF BOOST_PP_IIF

#  include <boost/preprocessor/comparison.hpp>
#  define ARX_EQUAL BOOST_PP_EQUAL
#  define ARX_NOT_EQUAL BOOST_PP_NOT_EQUAL

#  include <boost/preprocessor/array.hpp>
#  define ARX_ARRAY_SIZE BOOST_PP_ARRAY_SIZE
#  define ARX_ARRAY_DATA BOOST_PP_ARRAY_DATA
#  define ARX_ARRAY_ELEM BOOST_PP_ARRAY_ELEM
#  define ARX_ARRAY_REVERSE BOOST_PP_ARRAY_REVERSE

#  include <boost/preprocessor/tuple.hpp>
#  define ARX_TUPLE_ELEM BOOST_PP_TUPLE_ELEM
#  define ARX_TUPLE_REVERSE BOOST_PP_TUPLE_REVERSE
#  define ARX_TUPLE_EAT BOOST_PP_TUPLE_EAT

#  include <boost/preprocessor/detail/auto_rec.hpp>
#  define ARX_AUTO_REC BOOST_PP_AUTO_REC

#  include <boost/preprocessor/punctuation.hpp>
#  define ARX_COMMA BOOST_PP_COMMA
#  define ARX_LPAREN BOOST_PP_LPAREN
#  define ARX_RPAREN BOOST_PP_RPAREN
#  define ARX_COMMA_IF BOOST_PP_COMMA_IF
#  define ARX_LPAREN_IF BOOST_PP_LPAREN_IF
#  define ARX_RPAREN_IF BOOST_PP_RPAREN_IF

#  include <boost/preprocessor/facilities.hpp>
#  define ARX_EMPTY BOOST_PP_EMPTY
#  define ARX_INTERCEPT BOOST_PP_INTERCEPT
#  define ARX_IS_EMPTY BOOST_PP_IS_EMPTY
#else // ARX_USE_BOOST


/**
 * Empty
 */
#  define ARX_EMPTY()


/**
 * Comma
 */
#  define ARX_COMMA() ,


/**
 * LParen
 */
#  define ARX_LPAREN() (


/**
 * RParen
 */
#  define ARX_RPAREN() )


/**
 * Comma if COND is true (i.e. != 0), empty otherwise
 */
#  define ARX_COMMA_IF(COND) ARX_IF(COND, ARX_COMMA, ARX_EMPTY)()


/**
 * LParen if COND is true (i.e. != 0), empty otherwise
 */
#  define ARX_LPAREN_IF(COND) ARX_IF(COND, ARX_LPAREN, ARX_EMPTY)()


/**
 * RParen if COND is true (i.e. != 0), empty otherwise
 */
#  define ARX_RPAREN_IF(COND) ARX_IF(COND, ARX_RPAREN, ARX_EMPTY)()


/**
 * Joins the two arguments together, even when one of the arguments is itself a macro
 */
#  define ARX_JOIN(a, b) ARX_JOIN_I(a, b)
#  define ARX_JOIN_I(a, b) ARX_JOIN_II(a ## b)
#  define ARX_JOIN_II(res) res


/**
 * Converts the parameter X to a string after macro replacement on X has been performed.
 */
#  define ARX_STRINGIZE(X) ARX_STRINGIZE_I(X)
#  define ARX_STRINGIZE_I(X) #X


/**
 * This brain-explosive macro finds the first k in [1..N] for which PRED returns 1. <br>
 * Please note that this macro relies on the fact that PRED returns 1 only for arguments which lie in [1..M] segment (possibly empty), M < N. <br>
 * Also note that N must be a power of 2. <br>
 * Currently only N <= 8 is supported. <br>
 */
#  define ARX_AUTO_REC(PRED, N) ARX_BSEARCH_ ## N(PRED)
#  define ARX_BSEARCH_8(P) ARX_SNODE_4(P)(P)(P)
#  define ARX_BSEARCH_4(P) ARX_SNODE_2(P)(P)
#  define ARX_BSEARCH_2(P) ARX_SNODE_1(P)

#  define ARX_SNODE_1(P) ARX_IIF(P(1), 1, 2)
#  define ARX_SNODE_2(P) ARX_IIF(P(2), ARX_SNODE_1, ARX_SNODE_3)
#  define ARX_SNODE_3(P) ARX_IIF(P(3), 3, 4)
#  define ARX_SNODE_4(P) ARX_IIF(P(4), ARX_SNODE_2, ARX_SNODE_6)
#  define ARX_SNODE_5(P) ARX_IIF(P(5), 5, 6)
#  define ARX_SNODE_6(P) ARX_IIF(P(6), ARX_SNODE_5, ARX_SNODE_7)
#  define ARX_SNODE_7(P) ARX_IIF(P(7), 7, 8)


/**
 * This macro returns 1 if x evaluates to empty token, 0 otherwise.
 * Please note that x must not contain comma characters.
 */
#  define ARX_IS_EMPTY(x) ARX_IS_EMPTY_I(ARX_IS_EMPTY_HELPER x ())
#  define ARX_IS_EMPTY_I(test) ARX_IS_EMPTY_II(ARX_TUPLE_ELEM(2, 0, (ARX_JOIN(ARX_IS_EMPTY_DEF_, test))))
#  define ARX_IS_EMPTY_II(id) id

#  define ARX_IS_EMPTY_HELPER() 1
#  define ARX_IS_EMPTY_DEF_1 1, ARX_NIL
#  define ARX_IS_EMPTY_DEF_ARX_IS_EMPTY_HELPER 0, ARX_NIL


/**
 * Converts the given INT (i.e. 0 to 256) to BOOL (i.e 0 or 1)
 */
#  define ARX_BOOL(x) ARX_BOOL_I(x)
#  define ARX_BOOL_I(x) ARX_BOOL_II(ARX_BOOL_ ## x)
#  define ARX_BOOL_II(res) res

#  define ARX_BOOL_0 0
#  define ARX_BOOL_1 1
#  define ARX_BOOL_2 1
#  define ARX_BOOL_3 1
#  define ARX_BOOL_4 1
#  define ARX_BOOL_5 1
#  define ARX_BOOL_6 1
#  define ARX_BOOL_7 1
#  define ARX_BOOL_8 1
#  define ARX_BOOL_9 1
#  define ARX_BOOL_10 1
#  define ARX_BOOL_11 1
#  define ARX_BOOL_12 1
#  define ARX_BOOL_13 1
#  define ARX_BOOL_14 1
#  define ARX_BOOL_15 1
#  define ARX_BOOL_16 1
#  define ARX_BOOL_17 1
#  define ARX_BOOL_18 1
#  define ARX_BOOL_19 1
#  define ARX_BOOL_20 1
#  define ARX_BOOL_21 1
#  define ARX_BOOL_22 1
#  define ARX_BOOL_23 1
#  define ARX_BOOL_24 1
#  define ARX_BOOL_25 1
#  define ARX_BOOL_26 1
#  define ARX_BOOL_27 1
#  define ARX_BOOL_28 1
#  define ARX_BOOL_29 1
#  define ARX_BOOL_30 1
#  define ARX_BOOL_31 1
#  define ARX_BOOL_32 1
#  define ARX_BOOL_33 1
#  define ARX_BOOL_34 1
#  define ARX_BOOL_35 1
#  define ARX_BOOL_36 1
#  define ARX_BOOL_37 1
#  define ARX_BOOL_38 1
#  define ARX_BOOL_39 1
#  define ARX_BOOL_40 1
#  define ARX_BOOL_41 1
#  define ARX_BOOL_42 1
#  define ARX_BOOL_43 1
#  define ARX_BOOL_44 1
#  define ARX_BOOL_45 1
#  define ARX_BOOL_46 1
#  define ARX_BOOL_47 1
#  define ARX_BOOL_48 1
#  define ARX_BOOL_49 1
#  define ARX_BOOL_50 1
#  define ARX_BOOL_51 1
#  define ARX_BOOL_52 1
#  define ARX_BOOL_53 1
#  define ARX_BOOL_54 1
#  define ARX_BOOL_55 1
#  define ARX_BOOL_56 1
#  define ARX_BOOL_57 1
#  define ARX_BOOL_58 1
#  define ARX_BOOL_59 1
#  define ARX_BOOL_60 1
#  define ARX_BOOL_61 1
#  define ARX_BOOL_62 1
#  define ARX_BOOL_63 1
#  define ARX_BOOL_64 1
#  define ARX_BOOL_65 1
#  define ARX_BOOL_66 1
#  define ARX_BOOL_67 1
#  define ARX_BOOL_68 1
#  define ARX_BOOL_69 1
#  define ARX_BOOL_70 1
#  define ARX_BOOL_71 1
#  define ARX_BOOL_72 1
#  define ARX_BOOL_73 1
#  define ARX_BOOL_74 1
#  define ARX_BOOL_75 1
#  define ARX_BOOL_76 1
#  define ARX_BOOL_77 1
#  define ARX_BOOL_78 1
#  define ARX_BOOL_79 1
#  define ARX_BOOL_80 1
#  define ARX_BOOL_81 1
#  define ARX_BOOL_82 1
#  define ARX_BOOL_83 1
#  define ARX_BOOL_84 1
#  define ARX_BOOL_85 1
#  define ARX_BOOL_86 1
#  define ARX_BOOL_87 1
#  define ARX_BOOL_88 1
#  define ARX_BOOL_89 1
#  define ARX_BOOL_90 1
#  define ARX_BOOL_91 1
#  define ARX_BOOL_92 1
#  define ARX_BOOL_93 1
#  define ARX_BOOL_94 1
#  define ARX_BOOL_95 1
#  define ARX_BOOL_96 1
#  define ARX_BOOL_97 1
#  define ARX_BOOL_98 1
#  define ARX_BOOL_99 1
#  define ARX_BOOL_100 1
#  define ARX_BOOL_101 1
#  define ARX_BOOL_102 1
#  define ARX_BOOL_103 1
#  define ARX_BOOL_104 1
#  define ARX_BOOL_105 1
#  define ARX_BOOL_106 1
#  define ARX_BOOL_107 1
#  define ARX_BOOL_108 1
#  define ARX_BOOL_109 1
#  define ARX_BOOL_110 1
#  define ARX_BOOL_111 1
#  define ARX_BOOL_112 1
#  define ARX_BOOL_113 1
#  define ARX_BOOL_114 1
#  define ARX_BOOL_115 1
#  define ARX_BOOL_116 1
#  define ARX_BOOL_117 1
#  define ARX_BOOL_118 1
#  define ARX_BOOL_119 1
#  define ARX_BOOL_120 1
#  define ARX_BOOL_121 1
#  define ARX_BOOL_122 1
#  define ARX_BOOL_123 1
#  define ARX_BOOL_124 1
#  define ARX_BOOL_125 1
#  define ARX_BOOL_126 1
#  define ARX_BOOL_127 1
#  define ARX_BOOL_128 1
#  define ARX_BOOL_129 1
#  define ARX_BOOL_130 1
#  define ARX_BOOL_131 1
#  define ARX_BOOL_132 1
#  define ARX_BOOL_133 1
#  define ARX_BOOL_134 1
#  define ARX_BOOL_135 1
#  define ARX_BOOL_136 1
#  define ARX_BOOL_137 1
#  define ARX_BOOL_138 1
#  define ARX_BOOL_139 1
#  define ARX_BOOL_140 1
#  define ARX_BOOL_141 1
#  define ARX_BOOL_142 1
#  define ARX_BOOL_143 1
#  define ARX_BOOL_144 1
#  define ARX_BOOL_145 1
#  define ARX_BOOL_146 1
#  define ARX_BOOL_147 1
#  define ARX_BOOL_148 1
#  define ARX_BOOL_149 1
#  define ARX_BOOL_150 1
#  define ARX_BOOL_151 1
#  define ARX_BOOL_152 1
#  define ARX_BOOL_153 1
#  define ARX_BOOL_154 1
#  define ARX_BOOL_155 1
#  define ARX_BOOL_156 1
#  define ARX_BOOL_157 1
#  define ARX_BOOL_158 1
#  define ARX_BOOL_159 1
#  define ARX_BOOL_160 1
#  define ARX_BOOL_161 1
#  define ARX_BOOL_162 1
#  define ARX_BOOL_163 1
#  define ARX_BOOL_164 1
#  define ARX_BOOL_165 1
#  define ARX_BOOL_166 1
#  define ARX_BOOL_167 1
#  define ARX_BOOL_168 1
#  define ARX_BOOL_169 1
#  define ARX_BOOL_170 1
#  define ARX_BOOL_171 1
#  define ARX_BOOL_172 1
#  define ARX_BOOL_173 1
#  define ARX_BOOL_174 1
#  define ARX_BOOL_175 1
#  define ARX_BOOL_176 1
#  define ARX_BOOL_177 1
#  define ARX_BOOL_178 1
#  define ARX_BOOL_179 1
#  define ARX_BOOL_180 1
#  define ARX_BOOL_181 1
#  define ARX_BOOL_182 1
#  define ARX_BOOL_183 1
#  define ARX_BOOL_184 1
#  define ARX_BOOL_185 1
#  define ARX_BOOL_186 1
#  define ARX_BOOL_187 1
#  define ARX_BOOL_188 1
#  define ARX_BOOL_189 1
#  define ARX_BOOL_190 1
#  define ARX_BOOL_191 1
#  define ARX_BOOL_192 1
#  define ARX_BOOL_193 1
#  define ARX_BOOL_194 1
#  define ARX_BOOL_195 1
#  define ARX_BOOL_196 1
#  define ARX_BOOL_197 1
#  define ARX_BOOL_198 1
#  define ARX_BOOL_199 1
#  define ARX_BOOL_200 1
#  define ARX_BOOL_201 1
#  define ARX_BOOL_202 1
#  define ARX_BOOL_203 1
#  define ARX_BOOL_204 1
#  define ARX_BOOL_205 1
#  define ARX_BOOL_206 1
#  define ARX_BOOL_207 1
#  define ARX_BOOL_208 1
#  define ARX_BOOL_209 1
#  define ARX_BOOL_210 1
#  define ARX_BOOL_211 1
#  define ARX_BOOL_212 1
#  define ARX_BOOL_213 1
#  define ARX_BOOL_214 1
#  define ARX_BOOL_215 1
#  define ARX_BOOL_216 1
#  define ARX_BOOL_217 1
#  define ARX_BOOL_218 1
#  define ARX_BOOL_219 1
#  define ARX_BOOL_220 1
#  define ARX_BOOL_221 1
#  define ARX_BOOL_222 1
#  define ARX_BOOL_223 1
#  define ARX_BOOL_224 1
#  define ARX_BOOL_225 1
#  define ARX_BOOL_226 1
#  define ARX_BOOL_227 1
#  define ARX_BOOL_228 1
#  define ARX_BOOL_229 1
#  define ARX_BOOL_230 1
#  define ARX_BOOL_231 1
#  define ARX_BOOL_232 1
#  define ARX_BOOL_233 1
#  define ARX_BOOL_234 1
#  define ARX_BOOL_235 1
#  define ARX_BOOL_236 1
#  define ARX_BOOL_237 1
#  define ARX_BOOL_238 1
#  define ARX_BOOL_239 1
#  define ARX_BOOL_240 1
#  define ARX_BOOL_241 1
#  define ARX_BOOL_242 1
#  define ARX_BOOL_243 1
#  define ARX_BOOL_244 1
#  define ARX_BOOL_245 1
#  define ARX_BOOL_246 1
#  define ARX_BOOL_247 1
#  define ARX_BOOL_248 1
#  define ARX_BOOL_249 1
#  define ARX_BOOL_250 1
#  define ARX_BOOL_251 1
#  define ARX_BOOL_252 1
#  define ARX_BOOL_253 1
#  define ARX_BOOL_254 1
#  define ARX_BOOL_255 1
#  define ARX_BOOL_256 1


/**
 * Eats the following number when joined with it
 */
#  define ARX_INTERCEPT ARX_INTERCEPT_

#  define ARX_INTERCEPT_0
#  define ARX_INTERCEPT_1
#  define ARX_INTERCEPT_2
#  define ARX_INTERCEPT_3
#  define ARX_INTERCEPT_4
#  define ARX_INTERCEPT_5
#  define ARX_INTERCEPT_6
#  define ARX_INTERCEPT_7
#  define ARX_INTERCEPT_8
#  define ARX_INTERCEPT_9
#  define ARX_INTERCEPT_10
#  define ARX_INTERCEPT_11
#  define ARX_INTERCEPT_12
#  define ARX_INTERCEPT_13
#  define ARX_INTERCEPT_14
#  define ARX_INTERCEPT_15
#  define ARX_INTERCEPT_16
#  define ARX_INTERCEPT_17
#  define ARX_INTERCEPT_18
#  define ARX_INTERCEPT_19
#  define ARX_INTERCEPT_20
#  define ARX_INTERCEPT_21
#  define ARX_INTERCEPT_22
#  define ARX_INTERCEPT_23
#  define ARX_INTERCEPT_24
#  define ARX_INTERCEPT_25
#  define ARX_INTERCEPT_26
#  define ARX_INTERCEPT_27
#  define ARX_INTERCEPT_28
#  define ARX_INTERCEPT_29
#  define ARX_INTERCEPT_30
#  define ARX_INTERCEPT_31
#  define ARX_INTERCEPT_32
#  define ARX_INTERCEPT_33
#  define ARX_INTERCEPT_34
#  define ARX_INTERCEPT_35
#  define ARX_INTERCEPT_36
#  define ARX_INTERCEPT_37
#  define ARX_INTERCEPT_38
#  define ARX_INTERCEPT_39
#  define ARX_INTERCEPT_40
#  define ARX_INTERCEPT_41
#  define ARX_INTERCEPT_42
#  define ARX_INTERCEPT_43
#  define ARX_INTERCEPT_44
#  define ARX_INTERCEPT_45
#  define ARX_INTERCEPT_46
#  define ARX_INTERCEPT_47
#  define ARX_INTERCEPT_48
#  define ARX_INTERCEPT_49
#  define ARX_INTERCEPT_50
#  define ARX_INTERCEPT_51
#  define ARX_INTERCEPT_52
#  define ARX_INTERCEPT_53
#  define ARX_INTERCEPT_54
#  define ARX_INTERCEPT_55
#  define ARX_INTERCEPT_56
#  define ARX_INTERCEPT_57
#  define ARX_INTERCEPT_58
#  define ARX_INTERCEPT_59
#  define ARX_INTERCEPT_60
#  define ARX_INTERCEPT_61
#  define ARX_INTERCEPT_62
#  define ARX_INTERCEPT_63
#  define ARX_INTERCEPT_64
#  define ARX_INTERCEPT_65
#  define ARX_INTERCEPT_66
#  define ARX_INTERCEPT_67
#  define ARX_INTERCEPT_68
#  define ARX_INTERCEPT_69
#  define ARX_INTERCEPT_70
#  define ARX_INTERCEPT_71
#  define ARX_INTERCEPT_72
#  define ARX_INTERCEPT_73
#  define ARX_INTERCEPT_74
#  define ARX_INTERCEPT_75
#  define ARX_INTERCEPT_76
#  define ARX_INTERCEPT_77
#  define ARX_INTERCEPT_78
#  define ARX_INTERCEPT_79
#  define ARX_INTERCEPT_80
#  define ARX_INTERCEPT_81
#  define ARX_INTERCEPT_82
#  define ARX_INTERCEPT_83
#  define ARX_INTERCEPT_84
#  define ARX_INTERCEPT_85
#  define ARX_INTERCEPT_86
#  define ARX_INTERCEPT_87
#  define ARX_INTERCEPT_88
#  define ARX_INTERCEPT_89
#  define ARX_INTERCEPT_90
#  define ARX_INTERCEPT_91
#  define ARX_INTERCEPT_92
#  define ARX_INTERCEPT_93
#  define ARX_INTERCEPT_94
#  define ARX_INTERCEPT_95
#  define ARX_INTERCEPT_96
#  define ARX_INTERCEPT_97
#  define ARX_INTERCEPT_98
#  define ARX_INTERCEPT_99
#  define ARX_INTERCEPT_100
#  define ARX_INTERCEPT_101
#  define ARX_INTERCEPT_102
#  define ARX_INTERCEPT_103
#  define ARX_INTERCEPT_104
#  define ARX_INTERCEPT_105
#  define ARX_INTERCEPT_106
#  define ARX_INTERCEPT_107
#  define ARX_INTERCEPT_108
#  define ARX_INTERCEPT_109
#  define ARX_INTERCEPT_110
#  define ARX_INTERCEPT_111
#  define ARX_INTERCEPT_112
#  define ARX_INTERCEPT_113
#  define ARX_INTERCEPT_114
#  define ARX_INTERCEPT_115
#  define ARX_INTERCEPT_116
#  define ARX_INTERCEPT_117
#  define ARX_INTERCEPT_118
#  define ARX_INTERCEPT_119
#  define ARX_INTERCEPT_120
#  define ARX_INTERCEPT_121
#  define ARX_INTERCEPT_122
#  define ARX_INTERCEPT_123
#  define ARX_INTERCEPT_124
#  define ARX_INTERCEPT_125
#  define ARX_INTERCEPT_126
#  define ARX_INTERCEPT_127
#  define ARX_INTERCEPT_128
#  define ARX_INTERCEPT_129
#  define ARX_INTERCEPT_130
#  define ARX_INTERCEPT_131
#  define ARX_INTERCEPT_132
#  define ARX_INTERCEPT_133
#  define ARX_INTERCEPT_134
#  define ARX_INTERCEPT_135
#  define ARX_INTERCEPT_136
#  define ARX_INTERCEPT_137
#  define ARX_INTERCEPT_138
#  define ARX_INTERCEPT_139
#  define ARX_INTERCEPT_140
#  define ARX_INTERCEPT_141
#  define ARX_INTERCEPT_142
#  define ARX_INTERCEPT_143
#  define ARX_INTERCEPT_144
#  define ARX_INTERCEPT_145
#  define ARX_INTERCEPT_146
#  define ARX_INTERCEPT_147
#  define ARX_INTERCEPT_148
#  define ARX_INTERCEPT_149
#  define ARX_INTERCEPT_150
#  define ARX_INTERCEPT_151
#  define ARX_INTERCEPT_152
#  define ARX_INTERCEPT_153
#  define ARX_INTERCEPT_154
#  define ARX_INTERCEPT_155
#  define ARX_INTERCEPT_156
#  define ARX_INTERCEPT_157
#  define ARX_INTERCEPT_158
#  define ARX_INTERCEPT_159
#  define ARX_INTERCEPT_160
#  define ARX_INTERCEPT_161
#  define ARX_INTERCEPT_162
#  define ARX_INTERCEPT_163
#  define ARX_INTERCEPT_164
#  define ARX_INTERCEPT_165
#  define ARX_INTERCEPT_166
#  define ARX_INTERCEPT_167
#  define ARX_INTERCEPT_168
#  define ARX_INTERCEPT_169
#  define ARX_INTERCEPT_170
#  define ARX_INTERCEPT_171
#  define ARX_INTERCEPT_172
#  define ARX_INTERCEPT_173
#  define ARX_INTERCEPT_174
#  define ARX_INTERCEPT_175
#  define ARX_INTERCEPT_176
#  define ARX_INTERCEPT_177
#  define ARX_INTERCEPT_178
#  define ARX_INTERCEPT_179
#  define ARX_INTERCEPT_180
#  define ARX_INTERCEPT_181
#  define ARX_INTERCEPT_182
#  define ARX_INTERCEPT_183
#  define ARX_INTERCEPT_184
#  define ARX_INTERCEPT_185
#  define ARX_INTERCEPT_186
#  define ARX_INTERCEPT_187
#  define ARX_INTERCEPT_188
#  define ARX_INTERCEPT_189
#  define ARX_INTERCEPT_190
#  define ARX_INTERCEPT_191
#  define ARX_INTERCEPT_192
#  define ARX_INTERCEPT_193
#  define ARX_INTERCEPT_194
#  define ARX_INTERCEPT_195
#  define ARX_INTERCEPT_196
#  define ARX_INTERCEPT_197
#  define ARX_INTERCEPT_198
#  define ARX_INTERCEPT_199
#  define ARX_INTERCEPT_200
#  define ARX_INTERCEPT_201
#  define ARX_INTERCEPT_202
#  define ARX_INTERCEPT_203
#  define ARX_INTERCEPT_204
#  define ARX_INTERCEPT_205
#  define ARX_INTERCEPT_206
#  define ARX_INTERCEPT_207
#  define ARX_INTERCEPT_208
#  define ARX_INTERCEPT_209
#  define ARX_INTERCEPT_210
#  define ARX_INTERCEPT_211
#  define ARX_INTERCEPT_212
#  define ARX_INTERCEPT_213
#  define ARX_INTERCEPT_214
#  define ARX_INTERCEPT_215
#  define ARX_INTERCEPT_216
#  define ARX_INTERCEPT_217
#  define ARX_INTERCEPT_218
#  define ARX_INTERCEPT_219
#  define ARX_INTERCEPT_220
#  define ARX_INTERCEPT_221
#  define ARX_INTERCEPT_222
#  define ARX_INTERCEPT_223
#  define ARX_INTERCEPT_224
#  define ARX_INTERCEPT_225
#  define ARX_INTERCEPT_226
#  define ARX_INTERCEPT_227
#  define ARX_INTERCEPT_228
#  define ARX_INTERCEPT_229
#  define ARX_INTERCEPT_230
#  define ARX_INTERCEPT_231
#  define ARX_INTERCEPT_232
#  define ARX_INTERCEPT_233
#  define ARX_INTERCEPT_234
#  define ARX_INTERCEPT_235
#  define ARX_INTERCEPT_236
#  define ARX_INTERCEPT_237
#  define ARX_INTERCEPT_238
#  define ARX_INTERCEPT_239
#  define ARX_INTERCEPT_240
#  define ARX_INTERCEPT_241
#  define ARX_INTERCEPT_242
#  define ARX_INTERCEPT_243
#  define ARX_INTERCEPT_244
#  define ARX_INTERCEPT_245
#  define ARX_INTERCEPT_246
#  define ARX_INTERCEPT_247
#  define ARX_INTERCEPT_248
#  define ARX_INTERCEPT_249
#  define ARX_INTERCEPT_250
#  define ARX_INTERCEPT_251
#  define ARX_INTERCEPT_252
#  define ARX_INTERCEPT_253
#  define ARX_INTERCEPT_254
#  define ARX_INTERCEPT_255
#  define ARX_INTERCEPT_256


/**
 * Increment
 */
#  define ARX_INC(x) ARX_INC_I(x)
#  define ARX_INC_I(x) ARX_INC_ ## x

#  define ARX_INC_0 1
#  define ARX_INC_1 2
#  define ARX_INC_2 3
#  define ARX_INC_3 4
#  define ARX_INC_4 5
#  define ARX_INC_5 6
#  define ARX_INC_6 7
#  define ARX_INC_7 8
#  define ARX_INC_8 9
#  define ARX_INC_9 10
#  define ARX_INC_10 11
#  define ARX_INC_11 12
#  define ARX_INC_12 13
#  define ARX_INC_13 14
#  define ARX_INC_14 15
#  define ARX_INC_15 16
#  define ARX_INC_16 17
#  define ARX_INC_17 18
#  define ARX_INC_18 19
#  define ARX_INC_19 20
#  define ARX_INC_20 21
#  define ARX_INC_21 22
#  define ARX_INC_22 23
#  define ARX_INC_23 24
#  define ARX_INC_24 25
#  define ARX_INC_25 26
#  define ARX_INC_26 27
#  define ARX_INC_27 28
#  define ARX_INC_28 29
#  define ARX_INC_29 30
#  define ARX_INC_30 31
#  define ARX_INC_31 32
#  define ARX_INC_32 33
#  define ARX_INC_33 34
#  define ARX_INC_34 35
#  define ARX_INC_35 36
#  define ARX_INC_36 37
#  define ARX_INC_37 38
#  define ARX_INC_38 39
#  define ARX_INC_39 40
#  define ARX_INC_40 41
#  define ARX_INC_41 42
#  define ARX_INC_42 43
#  define ARX_INC_43 44
#  define ARX_INC_44 45
#  define ARX_INC_45 46
#  define ARX_INC_46 47
#  define ARX_INC_47 48
#  define ARX_INC_48 49
#  define ARX_INC_49 50
#  define ARX_INC_50 51
#  define ARX_INC_51 52
#  define ARX_INC_52 53
#  define ARX_INC_53 54
#  define ARX_INC_54 55
#  define ARX_INC_55 56
#  define ARX_INC_56 57
#  define ARX_INC_57 58
#  define ARX_INC_58 59
#  define ARX_INC_59 60
#  define ARX_INC_60 61
#  define ARX_INC_61 62
#  define ARX_INC_62 63
#  define ARX_INC_63 64
#  define ARX_INC_64 65
#  define ARX_INC_65 66
#  define ARX_INC_66 67
#  define ARX_INC_67 68
#  define ARX_INC_68 69
#  define ARX_INC_69 70
#  define ARX_INC_70 71
#  define ARX_INC_71 72
#  define ARX_INC_72 73
#  define ARX_INC_73 74
#  define ARX_INC_74 75
#  define ARX_INC_75 76
#  define ARX_INC_76 77
#  define ARX_INC_77 78
#  define ARX_INC_78 79
#  define ARX_INC_79 80
#  define ARX_INC_80 81
#  define ARX_INC_81 82
#  define ARX_INC_82 83
#  define ARX_INC_83 84
#  define ARX_INC_84 85
#  define ARX_INC_85 86
#  define ARX_INC_86 87
#  define ARX_INC_87 88
#  define ARX_INC_88 89
#  define ARX_INC_89 90
#  define ARX_INC_90 91
#  define ARX_INC_91 92
#  define ARX_INC_92 93
#  define ARX_INC_93 94
#  define ARX_INC_94 95
#  define ARX_INC_95 96
#  define ARX_INC_96 97
#  define ARX_INC_97 98
#  define ARX_INC_98 99
#  define ARX_INC_99 100
#  define ARX_INC_100 101
#  define ARX_INC_101 102
#  define ARX_INC_102 103
#  define ARX_INC_103 104
#  define ARX_INC_104 105
#  define ARX_INC_105 106
#  define ARX_INC_106 107
#  define ARX_INC_107 108
#  define ARX_INC_108 109
#  define ARX_INC_109 110
#  define ARX_INC_110 111
#  define ARX_INC_111 112
#  define ARX_INC_112 113
#  define ARX_INC_113 114
#  define ARX_INC_114 115
#  define ARX_INC_115 116
#  define ARX_INC_116 117
#  define ARX_INC_117 118
#  define ARX_INC_118 119
#  define ARX_INC_119 120
#  define ARX_INC_120 121
#  define ARX_INC_121 122
#  define ARX_INC_122 123
#  define ARX_INC_123 124
#  define ARX_INC_124 125
#  define ARX_INC_125 126
#  define ARX_INC_126 127
#  define ARX_INC_127 128
#  define ARX_INC_128 129
#  define ARX_INC_129 130
#  define ARX_INC_130 131
#  define ARX_INC_131 132
#  define ARX_INC_132 133
#  define ARX_INC_133 134
#  define ARX_INC_134 135
#  define ARX_INC_135 136
#  define ARX_INC_136 137
#  define ARX_INC_137 138
#  define ARX_INC_138 139
#  define ARX_INC_139 140
#  define ARX_INC_140 141
#  define ARX_INC_141 142
#  define ARX_INC_142 143
#  define ARX_INC_143 144
#  define ARX_INC_144 145
#  define ARX_INC_145 146
#  define ARX_INC_146 147
#  define ARX_INC_147 148
#  define ARX_INC_148 149
#  define ARX_INC_149 150
#  define ARX_INC_150 151
#  define ARX_INC_151 152
#  define ARX_INC_152 153
#  define ARX_INC_153 154
#  define ARX_INC_154 155
#  define ARX_INC_155 156
#  define ARX_INC_156 157
#  define ARX_INC_157 158
#  define ARX_INC_158 159
#  define ARX_INC_159 160
#  define ARX_INC_160 161
#  define ARX_INC_161 162
#  define ARX_INC_162 163
#  define ARX_INC_163 164
#  define ARX_INC_164 165
#  define ARX_INC_165 166
#  define ARX_INC_166 167
#  define ARX_INC_167 168
#  define ARX_INC_168 169
#  define ARX_INC_169 170
#  define ARX_INC_170 171
#  define ARX_INC_171 172
#  define ARX_INC_172 173
#  define ARX_INC_173 174
#  define ARX_INC_174 175
#  define ARX_INC_175 176
#  define ARX_INC_176 177
#  define ARX_INC_177 178
#  define ARX_INC_178 179
#  define ARX_INC_179 180
#  define ARX_INC_180 181
#  define ARX_INC_181 182
#  define ARX_INC_182 183
#  define ARX_INC_183 184
#  define ARX_INC_184 185
#  define ARX_INC_185 186
#  define ARX_INC_186 187
#  define ARX_INC_187 188
#  define ARX_INC_188 189
#  define ARX_INC_189 190
#  define ARX_INC_190 191
#  define ARX_INC_191 192
#  define ARX_INC_192 193
#  define ARX_INC_193 194
#  define ARX_INC_194 195
#  define ARX_INC_195 196
#  define ARX_INC_196 197
#  define ARX_INC_197 198
#  define ARX_INC_198 199
#  define ARX_INC_199 200
#  define ARX_INC_200 201
#  define ARX_INC_201 202
#  define ARX_INC_202 203
#  define ARX_INC_203 204
#  define ARX_INC_204 205
#  define ARX_INC_205 206
#  define ARX_INC_206 207
#  define ARX_INC_207 208
#  define ARX_INC_208 209
#  define ARX_INC_209 210
#  define ARX_INC_210 211
#  define ARX_INC_211 212
#  define ARX_INC_212 213
#  define ARX_INC_213 214
#  define ARX_INC_214 215
#  define ARX_INC_215 216
#  define ARX_INC_216 217
#  define ARX_INC_217 218
#  define ARX_INC_218 219
#  define ARX_INC_219 220
#  define ARX_INC_220 221
#  define ARX_INC_221 222
#  define ARX_INC_222 223
#  define ARX_INC_223 224
#  define ARX_INC_224 225
#  define ARX_INC_225 226
#  define ARX_INC_226 227
#  define ARX_INC_227 228
#  define ARX_INC_228 229
#  define ARX_INC_229 230
#  define ARX_INC_230 231
#  define ARX_INC_231 232
#  define ARX_INC_232 233
#  define ARX_INC_233 234
#  define ARX_INC_234 235
#  define ARX_INC_235 236
#  define ARX_INC_236 237
#  define ARX_INC_237 238
#  define ARX_INC_238 239
#  define ARX_INC_239 240
#  define ARX_INC_240 241
#  define ARX_INC_241 242
#  define ARX_INC_242 243
#  define ARX_INC_243 244
#  define ARX_INC_244 245
#  define ARX_INC_245 246
#  define ARX_INC_246 247
#  define ARX_INC_247 248
#  define ARX_INC_248 249
#  define ARX_INC_249 250
#  define ARX_INC_250 251
#  define ARX_INC_251 252
#  define ARX_INC_252 253
#  define ARX_INC_253 254
#  define ARX_INC_254 255
#  define ARX_INC_255 256
#  define ARX_INC_256 256


/**
 * Decrement
 */
#  define ARX_DEC(x) ARX_DEC_I(x)
#  define ARX_DEC_I(x) ARX_DEC_ ## x

#  define ARX_DEC_0 0
#  define ARX_DEC_1 0
#  define ARX_DEC_2 1
#  define ARX_DEC_3 2
#  define ARX_DEC_4 3
#  define ARX_DEC_5 4
#  define ARX_DEC_6 5
#  define ARX_DEC_7 6
#  define ARX_DEC_8 7
#  define ARX_DEC_9 8
#  define ARX_DEC_10 9
#  define ARX_DEC_11 10
#  define ARX_DEC_12 11
#  define ARX_DEC_13 12
#  define ARX_DEC_14 13
#  define ARX_DEC_15 14
#  define ARX_DEC_16 15
#  define ARX_DEC_17 16
#  define ARX_DEC_18 17
#  define ARX_DEC_19 18
#  define ARX_DEC_20 19
#  define ARX_DEC_21 20
#  define ARX_DEC_22 21
#  define ARX_DEC_23 22
#  define ARX_DEC_24 23
#  define ARX_DEC_25 24
#  define ARX_DEC_26 25
#  define ARX_DEC_27 26
#  define ARX_DEC_28 27
#  define ARX_DEC_29 28
#  define ARX_DEC_30 29
#  define ARX_DEC_31 30
#  define ARX_DEC_32 31
#  define ARX_DEC_33 32
#  define ARX_DEC_34 33
#  define ARX_DEC_35 34
#  define ARX_DEC_36 35
#  define ARX_DEC_37 36
#  define ARX_DEC_38 37
#  define ARX_DEC_39 38
#  define ARX_DEC_40 39
#  define ARX_DEC_41 40
#  define ARX_DEC_42 41
#  define ARX_DEC_43 42
#  define ARX_DEC_44 43
#  define ARX_DEC_45 44
#  define ARX_DEC_46 45
#  define ARX_DEC_47 46
#  define ARX_DEC_48 47
#  define ARX_DEC_49 48
#  define ARX_DEC_50 49
#  define ARX_DEC_51 50
#  define ARX_DEC_52 51
#  define ARX_DEC_53 52
#  define ARX_DEC_54 53
#  define ARX_DEC_55 54
#  define ARX_DEC_56 55
#  define ARX_DEC_57 56
#  define ARX_DEC_58 57
#  define ARX_DEC_59 58
#  define ARX_DEC_60 59
#  define ARX_DEC_61 60
#  define ARX_DEC_62 61
#  define ARX_DEC_63 62
#  define ARX_DEC_64 63
#  define ARX_DEC_65 64
#  define ARX_DEC_66 65
#  define ARX_DEC_67 66
#  define ARX_DEC_68 67
#  define ARX_DEC_69 68
#  define ARX_DEC_70 69
#  define ARX_DEC_71 70
#  define ARX_DEC_72 71
#  define ARX_DEC_73 72
#  define ARX_DEC_74 73
#  define ARX_DEC_75 74
#  define ARX_DEC_76 75
#  define ARX_DEC_77 76
#  define ARX_DEC_78 77
#  define ARX_DEC_79 78
#  define ARX_DEC_80 79
#  define ARX_DEC_81 80
#  define ARX_DEC_82 81
#  define ARX_DEC_83 82
#  define ARX_DEC_84 83
#  define ARX_DEC_85 84
#  define ARX_DEC_86 85
#  define ARX_DEC_87 86
#  define ARX_DEC_88 87
#  define ARX_DEC_89 88
#  define ARX_DEC_90 89
#  define ARX_DEC_91 90
#  define ARX_DEC_92 91
#  define ARX_DEC_93 92
#  define ARX_DEC_94 93
#  define ARX_DEC_95 94
#  define ARX_DEC_96 95
#  define ARX_DEC_97 96
#  define ARX_DEC_98 97
#  define ARX_DEC_99 98
#  define ARX_DEC_100 99
#  define ARX_DEC_101 100
#  define ARX_DEC_102 101
#  define ARX_DEC_103 102
#  define ARX_DEC_104 103
#  define ARX_DEC_105 104
#  define ARX_DEC_106 105
#  define ARX_DEC_107 106
#  define ARX_DEC_108 107
#  define ARX_DEC_109 108
#  define ARX_DEC_110 109
#  define ARX_DEC_111 110
#  define ARX_DEC_112 111
#  define ARX_DEC_113 112
#  define ARX_DEC_114 113
#  define ARX_DEC_115 114
#  define ARX_DEC_116 115
#  define ARX_DEC_117 116
#  define ARX_DEC_118 117
#  define ARX_DEC_119 118
#  define ARX_DEC_120 119
#  define ARX_DEC_121 120
#  define ARX_DEC_122 121
#  define ARX_DEC_123 122
#  define ARX_DEC_124 123
#  define ARX_DEC_125 124
#  define ARX_DEC_126 125
#  define ARX_DEC_127 126
#  define ARX_DEC_128 127
#  define ARX_DEC_129 128
#  define ARX_DEC_130 129
#  define ARX_DEC_131 130
#  define ARX_DEC_132 131
#  define ARX_DEC_133 132
#  define ARX_DEC_134 133
#  define ARX_DEC_135 134
#  define ARX_DEC_136 135
#  define ARX_DEC_137 136
#  define ARX_DEC_138 137
#  define ARX_DEC_139 138
#  define ARX_DEC_140 139
#  define ARX_DEC_141 140
#  define ARX_DEC_142 141
#  define ARX_DEC_143 142
#  define ARX_DEC_144 143
#  define ARX_DEC_145 144
#  define ARX_DEC_146 145
#  define ARX_DEC_147 146
#  define ARX_DEC_148 147
#  define ARX_DEC_149 148
#  define ARX_DEC_150 149
#  define ARX_DEC_151 150
#  define ARX_DEC_152 151
#  define ARX_DEC_153 152
#  define ARX_DEC_154 153
#  define ARX_DEC_155 154
#  define ARX_DEC_156 155
#  define ARX_DEC_157 156
#  define ARX_DEC_158 157
#  define ARX_DEC_159 158
#  define ARX_DEC_160 159
#  define ARX_DEC_161 160
#  define ARX_DEC_162 161
#  define ARX_DEC_163 162
#  define ARX_DEC_164 163
#  define ARX_DEC_165 164
#  define ARX_DEC_166 165
#  define ARX_DEC_167 166
#  define ARX_DEC_168 167
#  define ARX_DEC_169 168
#  define ARX_DEC_170 169
#  define ARX_DEC_171 170
#  define ARX_DEC_172 171
#  define ARX_DEC_173 172
#  define ARX_DEC_174 173
#  define ARX_DEC_175 174
#  define ARX_DEC_176 175
#  define ARX_DEC_177 176
#  define ARX_DEC_178 177
#  define ARX_DEC_179 178
#  define ARX_DEC_180 179
#  define ARX_DEC_181 180
#  define ARX_DEC_182 181
#  define ARX_DEC_183 182
#  define ARX_DEC_184 183
#  define ARX_DEC_185 184
#  define ARX_DEC_186 185
#  define ARX_DEC_187 186
#  define ARX_DEC_188 187
#  define ARX_DEC_189 188
#  define ARX_DEC_190 189
#  define ARX_DEC_191 190
#  define ARX_DEC_192 191
#  define ARX_DEC_193 192
#  define ARX_DEC_194 193
#  define ARX_DEC_195 194
#  define ARX_DEC_196 195
#  define ARX_DEC_197 196
#  define ARX_DEC_198 197
#  define ARX_DEC_199 198
#  define ARX_DEC_200 199
#  define ARX_DEC_201 200
#  define ARX_DEC_202 201
#  define ARX_DEC_203 202
#  define ARX_DEC_204 203
#  define ARX_DEC_205 204
#  define ARX_DEC_206 205
#  define ARX_DEC_207 206
#  define ARX_DEC_208 207
#  define ARX_DEC_209 208
#  define ARX_DEC_210 209
#  define ARX_DEC_211 210
#  define ARX_DEC_212 211
#  define ARX_DEC_213 212
#  define ARX_DEC_214 213
#  define ARX_DEC_215 214
#  define ARX_DEC_216 215
#  define ARX_DEC_217 216
#  define ARX_DEC_218 217
#  define ARX_DEC_219 218
#  define ARX_DEC_220 219
#  define ARX_DEC_221 220
#  define ARX_DEC_222 221
#  define ARX_DEC_223 222
#  define ARX_DEC_224 223
#  define ARX_DEC_225 224
#  define ARX_DEC_226 225
#  define ARX_DEC_227 226
#  define ARX_DEC_228 227
#  define ARX_DEC_229 228
#  define ARX_DEC_230 229
#  define ARX_DEC_231 230
#  define ARX_DEC_232 231
#  define ARX_DEC_233 232
#  define ARX_DEC_234 233
#  define ARX_DEC_235 234
#  define ARX_DEC_236 235
#  define ARX_DEC_237 236
#  define ARX_DEC_238 237
#  define ARX_DEC_239 238
#  define ARX_DEC_240 239
#  define ARX_DEC_241 240
#  define ARX_DEC_242 241
#  define ARX_DEC_243 242
#  define ARX_DEC_244 243
#  define ARX_DEC_245 244
#  define ARX_DEC_246 245
#  define ARX_DEC_247 246
#  define ARX_DEC_248 247
#  define ARX_DEC_249 248
#  define ARX_DEC_250 249
#  define ARX_DEC_251 250
#  define ARX_DEC_252 251
#  define ARX_DEC_253 252
#  define ARX_DEC_254 253
#  define ARX_DEC_255 254
#  define ARX_DEC_256 255


/**
 * If BIT is 1 then evaluates to T, else to F.
 */
#  define ARX_IIF(BIT, T, F) ARX_IIF_I(BIT, T, F)
#  define ARX_IIF_I(BIT, T, F) ARX_IIF_II(ARX_IIF_ ## BIT(T, F))
#  define ARX_IIF_II(res) res
#  define ARX_IIF_0(T, F) F
#  define ARX_IIF_1(T, F) T


/**
 * If COND is true (i.e. != 0) then evaluates to T, else to F.
 */
#  define ARX_IF(COND, T, F) ARX_IIF(ARX_BOOL(COND), T, F)


/**
 * Bitor
 */
#  define ARX_BITOR(x, y) ARX_BITOR_I(x, y)
#  define ARX_BITOR_I(x, y) ARX_BITOR_II(ARX_BITOR_ ## x ## y)
#  define ARX_BITOR_II(res) res

#  define ARX_BITOR_00 0
#  define ARX_BITOR_01 1
#  define ARX_BITOR_10 1
#  define ARX_BITOR_11 1


/**
 * Bitand
 */
#  define ARX_BITAND(x, y) ARX_BITAND_I(x, y)
#  define ARX_BITAND_I(x, y) ARX_BITAND_II(ARX_BITAND_ ## x ## y)
#  define ARX_BITAND_II(res) res

#  define ARX_BITAND_00 0
#  define ARX_BITAND_01 0
#  define ARX_BITAND_10 0
#  define ARX_BITAND_11 1


/**
 * Bitnot
 */
#  define ARX_BITNOT(x) ARX_BITNOT_I(x)
#  define ARX_BITNOT_I(x) ARX_BITNOT_II(ARX_BITNOT_ ## x)
#  define ARX_BITNOT_II(id) id

#  define ARX_BITNOT_0 1
#  define ARX_BITNOT_1 0


/**
 * Or
 */
#  define ARX_OR(p, q) ARX_BITOR(ARX_BOOL(p), ARX_BOOL(q))


/**
 * And
 */
#  define ARX_AND(p, q) ARX_BITAND(ARX_BOOL(p), ARX_BOOL(q))


/**
 * Not
 */
#  define ARX_NOT(x) ARX_BITNOT(ARX_BOOL(x))


/**
 * Not equal
 */
#  define ARX_NOT_EQUAL(x, y) ARX_NOT_EQUAL_I(x, y)
#  define ARX_NOT_EQUAL_I(x, y) ARX_JOIN(ARX_NOT_EQUAL_CHECK_, ARX_NOT_EQUAL_ ## x(0, ARX_NOT_EQUAL_ ## y))

#  define ARX_NOT_EQUAL_CHECK_ARX_NIL 1
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_0(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_1(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_2(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_3(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_4(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_5(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_6(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_7(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_8(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_9(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_10(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_11(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_12(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_13(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_14(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_15(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_16(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_17(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_18(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_19(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_20(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_21(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_22(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_23(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_24(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_25(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_26(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_27(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_28(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_29(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_30(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_31(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_32(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_33(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_34(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_35(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_36(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_37(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_38(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_39(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_40(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_41(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_42(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_43(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_44(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_45(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_46(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_47(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_48(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_49(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_50(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_51(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_52(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_53(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_54(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_55(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_56(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_57(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_58(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_59(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_60(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_61(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_62(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_63(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_64(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_65(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_66(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_67(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_68(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_69(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_70(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_71(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_72(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_73(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_74(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_75(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_76(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_77(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_78(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_79(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_80(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_81(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_82(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_83(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_84(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_85(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_86(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_87(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_88(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_89(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_90(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_91(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_92(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_93(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_94(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_95(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_96(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_97(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_98(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_99(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_100(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_101(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_102(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_103(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_104(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_105(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_106(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_107(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_108(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_109(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_110(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_111(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_112(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_113(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_114(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_115(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_116(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_117(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_118(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_119(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_120(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_121(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_122(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_123(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_124(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_125(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_126(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_127(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_128(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_129(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_130(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_131(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_132(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_133(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_134(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_135(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_136(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_137(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_138(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_139(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_140(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_141(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_142(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_143(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_144(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_145(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_146(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_147(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_148(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_149(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_150(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_151(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_152(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_153(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_154(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_155(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_156(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_157(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_158(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_159(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_160(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_161(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_162(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_163(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_164(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_165(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_166(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_167(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_168(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_169(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_170(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_171(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_172(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_173(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_174(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_175(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_176(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_177(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_178(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_179(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_180(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_181(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_182(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_183(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_184(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_185(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_186(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_187(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_188(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_189(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_190(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_191(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_192(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_193(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_194(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_195(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_196(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_197(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_198(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_199(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_200(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_201(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_202(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_203(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_204(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_205(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_206(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_207(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_208(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_209(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_210(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_211(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_212(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_213(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_214(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_215(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_216(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_217(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_218(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_219(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_220(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_221(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_222(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_223(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_224(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_225(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_226(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_227(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_228(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_229(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_230(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_231(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_232(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_233(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_234(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_235(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_236(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_237(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_238(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_239(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_240(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_241(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_242(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_243(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_244(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_245(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_246(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_247(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_248(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_249(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_250(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_251(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_252(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_253(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_254(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_255(c, y) 0
#  define ARX_NOT_EQUAL_CHECK_ARX_NOT_EQUAL_256(c, y) 0

#  define ARX_NOT_EQUAL_0(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_1(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_2(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_3(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_4(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_5(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_6(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_7(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_8(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_9(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_10(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_11(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_12(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_13(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_14(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_15(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_16(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_17(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_18(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_19(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_20(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_21(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_22(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_23(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_24(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_25(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_26(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_27(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_28(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_29(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_30(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_31(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_32(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_33(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_34(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_35(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_36(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_37(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_38(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_39(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_40(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_41(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_42(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_43(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_44(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_45(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_46(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_47(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_48(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_49(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_50(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_51(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_52(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_53(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_54(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_55(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_56(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_57(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_58(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_59(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_60(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_61(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_62(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_63(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_64(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_65(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_66(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_67(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_68(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_69(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_70(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_71(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_72(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_73(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_74(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_75(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_76(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_77(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_78(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_79(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_80(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_81(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_82(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_83(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_84(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_85(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_86(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_87(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_88(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_89(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_90(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_91(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_92(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_93(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_94(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_95(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_96(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_97(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_98(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_99(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_100(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_101(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_102(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_103(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_104(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_105(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_106(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_107(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_108(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_109(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_110(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_111(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_112(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_113(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_114(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_115(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_116(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_117(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_118(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_119(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_120(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_121(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_122(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_123(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_124(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_125(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_126(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_127(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_128(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_129(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_130(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_131(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_132(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_133(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_134(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_135(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_136(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_137(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_138(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_139(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_140(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_141(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_142(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_143(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_144(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_145(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_146(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_147(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_148(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_149(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_150(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_151(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_152(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_153(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_154(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_155(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_156(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_157(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_158(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_159(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_160(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_161(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_162(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_163(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_164(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_165(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_166(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_167(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_168(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_169(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_170(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_171(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_172(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_173(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_174(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_175(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_176(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_177(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_178(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_179(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_180(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_181(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_182(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_183(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_184(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_185(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_186(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_187(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_188(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_189(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_190(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_191(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_192(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_193(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_194(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_195(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_196(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_197(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_198(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_199(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_200(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_201(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_202(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_203(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_204(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_205(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_206(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_207(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_208(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_209(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_210(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_211(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_212(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_213(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_214(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_215(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_216(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_217(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_218(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_219(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_220(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_221(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_222(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_223(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_224(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_225(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_226(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_227(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_228(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_229(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_230(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_231(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_232(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_233(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_234(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_235(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_236(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_237(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_238(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_239(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_240(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_241(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_242(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_243(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_244(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_245(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_246(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_247(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_248(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_249(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_250(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_251(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_252(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_253(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_254(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_255(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))
#  define ARX_NOT_EQUAL_256(c, y) ARX_IIF(c, ARX_NIL, y(1, ARX_NIL))


/**
 * Equal
 */
#  define ARX_EQUAL(x, y) ARX_BITNOT(ARX_NOT_EQUAL(x, y))


/**
 * Returns the size of a given array.
 * 
 * Array must be in the form <tt>(SIZE, (ELEMENTS))</tt>
 */
#  define ARX_ARRAY_SIZE(array) ARX_TUPLE_ELEM(2, 0, array)


/**
 * Returns the data of a given array as a tuple.
 *
 * Array must be in the form <tt>(SIZE, (ELEMENTS))</tt>
 */
#  define ARX_ARRAY_DATA(array) ARX_TUPLE_ELEM(2, 1, array)


/**
 * Returns an element of the given array.
 *
 * @param i index of an element needed
 * @param array array to extract element from
 */
#  define ARX_ARRAY_ELEM(i, array) ARX_TUPLE_ELEM(ARX_ARRAY_SIZE(array), i, ARX_ARRAY_DATA(array))


/**
 * Reverses the given array.
 */
#  define ARX_ARRAY_REVERSE(array) (ARX_ARRAY_SIZE(array), ARX_TUPLE_REVERSE(ARX_ARRAY_SIZE(array), ARX_ARRAY_DATA(array)))


/**
 * Generator for ARX_TYPLE_EAT_XX macro
 */
#  define ARX_TUPLE_EAT(size) ARX_TUPLE_EAT_I(size)
#  define ARX_TUPLE_EAT_I(size) ARX_TUPLE_EAT_ ## size

#  define ARX_TUPLE_EAT_0()
#  define ARX_TUPLE_EAT_1(a)
#  define ARX_TUPLE_EAT_2(a, b)
#  define ARX_TUPLE_EAT_3(a, b, c)
#  define ARX_TUPLE_EAT_4(a, b, c, d)
#  define ARX_TUPLE_EAT_5(a, b, c, d, e)
#  define ARX_TUPLE_EAT_6(a, b, c, d, e, f)
#  define ARX_TUPLE_EAT_7(a, b, c, d, e, f, g)
#  define ARX_TUPLE_EAT_8(a, b, c, d, e, f, g, h)
#  define ARX_TUPLE_EAT_9(a, b, c, d, e, f, g, h, i)
#  define ARX_TUPLE_EAT_10(a, b, c, d, e, f, g, h, i, j)
#  define ARX_TUPLE_EAT_11(a, b, c, d, e, f, g, h, i, j, k)
#  define ARX_TUPLE_EAT_12(a, b, c, d, e, f, g, h, i, j, k, l)
#  define ARX_TUPLE_EAT_13(a, b, c, d, e, f, g, h, i, j, k, l, m)
#  define ARX_TUPLE_EAT_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
#  define ARX_TUPLE_EAT_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)
#  define ARX_TUPLE_EAT_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)
#  define ARX_TUPLE_EAT_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)
#  define ARX_TUPLE_EAT_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r)
#  define ARX_TUPLE_EAT_19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s)
#  define ARX_TUPLE_EAT_20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t)
#  define ARX_TUPLE_EAT_21(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u)
#  define ARX_TUPLE_EAT_22(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v)
#  define ARX_TUPLE_EAT_23(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w)
#  define ARX_TUPLE_EAT_24(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x)
#  define ARX_TUPLE_EAT_25(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y)


/**
 * Reverses the given tuple.
 */
#  define ARX_TUPLE_REVERSE(size, tuple) ARX_TUPLE_REVERSE_I(size, tuple)
#  define ARX_TUPLE_REVERSE_I(s, t) ARX_TUPLE_REVERSE_II(ARX_TUPLE_REVERSE_ ## s t)
#  define ARX_TUPLE_REVERSE_II(res) res

#  define ARX_TUPLE_REVERSE_0() ()
#  define ARX_TUPLE_REVERSE_1(a) (a)
#  define ARX_TUPLE_REVERSE_2(a, b) (b, a)
#  define ARX_TUPLE_REVERSE_3(a, b, c) (c, b, a)
#  define ARX_TUPLE_REVERSE_4(a, b, c, d) (d, c, b, a)
#  define ARX_TUPLE_REVERSE_5(a, b, c, d, e) (e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_6(a, b, c, d, e, f) (f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_7(a, b, c, d, e, f, g) (g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_8(a, b, c, d, e, f, g, h) (h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_9(a, b, c, d, e, f, g, h, i) (i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_10(a, b, c, d, e, f, g, h, i, j) (j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_11(a, b, c, d, e, f, g, h, i, j, k) (k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_12(a, b, c, d, e, f, g, h, i, j, k, l) (l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_13(a, b, c, d, e, f, g, h, i, j, k, l, m) (m, l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n) (n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) (o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) (p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) (q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) (r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) (s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) (t, s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_21(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) (u, t, s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_22(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) (v, u, t, s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_23(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) (w, v, u, t, s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_24(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) (x, w, v, u, t, s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#  define ARX_TUPLE_REVERSE_25(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) (y, x, w, v, u, t, s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)


/**
 * Returns an element of the given tuple.
 *
 * @param size size of a given tuple
 * @param index index of an element needed
 * @param tuple tuple to extract element from
 */
#  define ARX_TUPLE_ELEM(size, index, tuple) ARX_TUPLE_ELEM_I(size, index, tuple)
#  define ARX_TUPLE_ELEM_I(s, i, t) ARX_TUPLE_ELEM_II(ARX_TUPLE_ELEM_ ## s ## _ ## i t)
#  define ARX_TUPLE_ELEM_II(res) res

#  define ARX_TUPLE_ELEM_1_0(a) a
 
#  define ARX_TUPLE_ELEM_2_0(a, b) a
#  define ARX_TUPLE_ELEM_2_1(a, b) b
 
#  define ARX_TUPLE_ELEM_3_0(a, b, c) a
#  define ARX_TUPLE_ELEM_3_1(a, b, c) b
#  define ARX_TUPLE_ELEM_3_2(a, b, c) c
 
#  define ARX_TUPLE_ELEM_4_0(a, b, c, d) a
#  define ARX_TUPLE_ELEM_4_1(a, b, c, d) b
#  define ARX_TUPLE_ELEM_4_2(a, b, c, d) c
#  define ARX_TUPLE_ELEM_4_3(a, b, c, d) d
 
#  define ARX_TUPLE_ELEM_5_0(a, b, c, d, e) a
#  define ARX_TUPLE_ELEM_5_1(a, b, c, d, e) b
#  define ARX_TUPLE_ELEM_5_2(a, b, c, d, e) c
#  define ARX_TUPLE_ELEM_5_3(a, b, c, d, e) d
#  define ARX_TUPLE_ELEM_5_4(a, b, c, d, e) e
 
#  define ARX_TUPLE_ELEM_6_0(a, b, c, d, e, f) a
#  define ARX_TUPLE_ELEM_6_1(a, b, c, d, e, f) b
#  define ARX_TUPLE_ELEM_6_2(a, b, c, d, e, f) c
#  define ARX_TUPLE_ELEM_6_3(a, b, c, d, e, f) d
#  define ARX_TUPLE_ELEM_6_4(a, b, c, d, e, f) e
#  define ARX_TUPLE_ELEM_6_5(a, b, c, d, e, f) f
 
#  define ARX_TUPLE_ELEM_7_0(a, b, c, d, e, f, g) a
#  define ARX_TUPLE_ELEM_7_1(a, b, c, d, e, f, g) b
#  define ARX_TUPLE_ELEM_7_2(a, b, c, d, e, f, g) c
#  define ARX_TUPLE_ELEM_7_3(a, b, c, d, e, f, g) d
#  define ARX_TUPLE_ELEM_7_4(a, b, c, d, e, f, g) e
#  define ARX_TUPLE_ELEM_7_5(a, b, c, d, e, f, g) f
#  define ARX_TUPLE_ELEM_7_6(a, b, c, d, e, f, g) g
 
#  define ARX_TUPLE_ELEM_8_0(a, b, c, d, e, f, g, h) a
#  define ARX_TUPLE_ELEM_8_1(a, b, c, d, e, f, g, h) b
#  define ARX_TUPLE_ELEM_8_2(a, b, c, d, e, f, g, h) c
#  define ARX_TUPLE_ELEM_8_3(a, b, c, d, e, f, g, h) d
#  define ARX_TUPLE_ELEM_8_4(a, b, c, d, e, f, g, h) e
#  define ARX_TUPLE_ELEM_8_5(a, b, c, d, e, f, g, h) f
#  define ARX_TUPLE_ELEM_8_6(a, b, c, d, e, f, g, h) g
#  define ARX_TUPLE_ELEM_8_7(a, b, c, d, e, f, g, h) h
 
#  define ARX_TUPLE_ELEM_9_0(a, b, c, d, e, f, g, h, i) a
#  define ARX_TUPLE_ELEM_9_1(a, b, c, d, e, f, g, h, i) b
#  define ARX_TUPLE_ELEM_9_2(a, b, c, d, e, f, g, h, i) c
#  define ARX_TUPLE_ELEM_9_3(a, b, c, d, e, f, g, h, i) d
#  define ARX_TUPLE_ELEM_9_4(a, b, c, d, e, f, g, h, i) e
#  define ARX_TUPLE_ELEM_9_5(a, b, c, d, e, f, g, h, i) f
#  define ARX_TUPLE_ELEM_9_6(a, b, c, d, e, f, g, h, i) g
#  define ARX_TUPLE_ELEM_9_7(a, b, c, d, e, f, g, h, i) h
#  define ARX_TUPLE_ELEM_9_8(a, b, c, d, e, f, g, h, i) i
 
#  define ARX_TUPLE_ELEM_10_0(a, b, c, d, e, f, g, h, i, j) a
#  define ARX_TUPLE_ELEM_10_1(a, b, c, d, e, f, g, h, i, j) b
#  define ARX_TUPLE_ELEM_10_2(a, b, c, d, e, f, g, h, i, j) c
#  define ARX_TUPLE_ELEM_10_3(a, b, c, d, e, f, g, h, i, j) d
#  define ARX_TUPLE_ELEM_10_4(a, b, c, d, e, f, g, h, i, j) e
#  define ARX_TUPLE_ELEM_10_5(a, b, c, d, e, f, g, h, i, j) f
#  define ARX_TUPLE_ELEM_10_6(a, b, c, d, e, f, g, h, i, j) g
#  define ARX_TUPLE_ELEM_10_7(a, b, c, d, e, f, g, h, i, j) h
#  define ARX_TUPLE_ELEM_10_8(a, b, c, d, e, f, g, h, i, j) i
#  define ARX_TUPLE_ELEM_10_9(a, b, c, d, e, f, g, h, i, j) j
 
#  define ARX_TUPLE_ELEM_11_0(a, b, c, d, e, f, g, h, i, j, k) a
#  define ARX_TUPLE_ELEM_11_1(a, b, c, d, e, f, g, h, i, j, k) b
#  define ARX_TUPLE_ELEM_11_2(a, b, c, d, e, f, g, h, i, j, k) c
#  define ARX_TUPLE_ELEM_11_3(a, b, c, d, e, f, g, h, i, j, k) d
#  define ARX_TUPLE_ELEM_11_4(a, b, c, d, e, f, g, h, i, j, k) e
#  define ARX_TUPLE_ELEM_11_5(a, b, c, d, e, f, g, h, i, j, k) f
#  define ARX_TUPLE_ELEM_11_6(a, b, c, d, e, f, g, h, i, j, k) g
#  define ARX_TUPLE_ELEM_11_7(a, b, c, d, e, f, g, h, i, j, k) h
#  define ARX_TUPLE_ELEM_11_8(a, b, c, d, e, f, g, h, i, j, k) i
#  define ARX_TUPLE_ELEM_11_9(a, b, c, d, e, f, g, h, i, j, k) j
#  define ARX_TUPLE_ELEM_11_10(a, b, c, d, e, f, g, h, i, j, k) k
 
#  define ARX_TUPLE_ELEM_12_0(a, b, c, d, e, f, g, h, i, j, k, l) a
#  define ARX_TUPLE_ELEM_12_1(a, b, c, d, e, f, g, h, i, j, k, l) b
#  define ARX_TUPLE_ELEM_12_2(a, b, c, d, e, f, g, h, i, j, k, l) c
#  define ARX_TUPLE_ELEM_12_3(a, b, c, d, e, f, g, h, i, j, k, l) d
#  define ARX_TUPLE_ELEM_12_4(a, b, c, d, e, f, g, h, i, j, k, l) e
#  define ARX_TUPLE_ELEM_12_5(a, b, c, d, e, f, g, h, i, j, k, l) f
#  define ARX_TUPLE_ELEM_12_6(a, b, c, d, e, f, g, h, i, j, k, l) g
#  define ARX_TUPLE_ELEM_12_7(a, b, c, d, e, f, g, h, i, j, k, l) h
#  define ARX_TUPLE_ELEM_12_8(a, b, c, d, e, f, g, h, i, j, k, l) i
#  define ARX_TUPLE_ELEM_12_9(a, b, c, d, e, f, g, h, i, j, k, l) j
#  define ARX_TUPLE_ELEM_12_10(a, b, c, d, e, f, g, h, i, j, k, l) k
#  define ARX_TUPLE_ELEM_12_11(a, b, c, d, e, f, g, h, i, j, k, l) l
 
#  define ARX_TUPLE_ELEM_13_0(a, b, c, d, e, f, g, h, i, j, k, l, m) a
#  define ARX_TUPLE_ELEM_13_1(a, b, c, d, e, f, g, h, i, j, k, l, m) b
#  define ARX_TUPLE_ELEM_13_2(a, b, c, d, e, f, g, h, i, j, k, l, m) c
#  define ARX_TUPLE_ELEM_13_3(a, b, c, d, e, f, g, h, i, j, k, l, m) d
#  define ARX_TUPLE_ELEM_13_4(a, b, c, d, e, f, g, h, i, j, k, l, m) e
#  define ARX_TUPLE_ELEM_13_5(a, b, c, d, e, f, g, h, i, j, k, l, m) f
#  define ARX_TUPLE_ELEM_13_6(a, b, c, d, e, f, g, h, i, j, k, l, m) g
#  define ARX_TUPLE_ELEM_13_7(a, b, c, d, e, f, g, h, i, j, k, l, m) h
#  define ARX_TUPLE_ELEM_13_8(a, b, c, d, e, f, g, h, i, j, k, l, m) i
#  define ARX_TUPLE_ELEM_13_9(a, b, c, d, e, f, g, h, i, j, k, l, m) j
#  define ARX_TUPLE_ELEM_13_10(a, b, c, d, e, f, g, h, i, j, k, l, m) k
#  define ARX_TUPLE_ELEM_13_11(a, b, c, d, e, f, g, h, i, j, k, l, m) l
#  define ARX_TUPLE_ELEM_13_12(a, b, c, d, e, f, g, h, i, j, k, l, m) m
 
#  define ARX_TUPLE_ELEM_14_0(a, b, c, d, e, f, g, h, i, j, k, l, m, n) a
#  define ARX_TUPLE_ELEM_14_1(a, b, c, d, e, f, g, h, i, j, k, l, m, n) b
#  define ARX_TUPLE_ELEM_14_2(a, b, c, d, e, f, g, h, i, j, k, l, m, n) c
#  define ARX_TUPLE_ELEM_14_3(a, b, c, d, e, f, g, h, i, j, k, l, m, n) d
#  define ARX_TUPLE_ELEM_14_4(a, b, c, d, e, f, g, h, i, j, k, l, m, n) e
#  define ARX_TUPLE_ELEM_14_5(a, b, c, d, e, f, g, h, i, j, k, l, m, n) f
#  define ARX_TUPLE_ELEM_14_6(a, b, c, d, e, f, g, h, i, j, k, l, m, n) g
#  define ARX_TUPLE_ELEM_14_7(a, b, c, d, e, f, g, h, i, j, k, l, m, n) h
#  define ARX_TUPLE_ELEM_14_8(a, b, c, d, e, f, g, h, i, j, k, l, m, n) i
#  define ARX_TUPLE_ELEM_14_9(a, b, c, d, e, f, g, h, i, j, k, l, m, n) j
#  define ARX_TUPLE_ELEM_14_10(a, b, c, d, e, f, g, h, i, j, k, l, m, n) k
#  define ARX_TUPLE_ELEM_14_11(a, b, c, d, e, f, g, h, i, j, k, l, m, n) l
#  define ARX_TUPLE_ELEM_14_12(a, b, c, d, e, f, g, h, i, j, k, l, m, n) m
#  define ARX_TUPLE_ELEM_14_13(a, b, c, d, e, f, g, h, i, j, k, l, m, n) n
  
#  define ARX_TUPLE_ELEM_15_0(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) a
#  define ARX_TUPLE_ELEM_15_1(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) b
#  define ARX_TUPLE_ELEM_15_2(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) c
#  define ARX_TUPLE_ELEM_15_3(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) d
#  define ARX_TUPLE_ELEM_15_4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) e
#  define ARX_TUPLE_ELEM_15_5(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) f
#  define ARX_TUPLE_ELEM_15_6(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) g
#  define ARX_TUPLE_ELEM_15_7(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) h
#  define ARX_TUPLE_ELEM_15_8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) i
#  define ARX_TUPLE_ELEM_15_9(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) j
#  define ARX_TUPLE_ELEM_15_10(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) k
#  define ARX_TUPLE_ELEM_15_11(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) l
#  define ARX_TUPLE_ELEM_15_12(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) m
#  define ARX_TUPLE_ELEM_15_13(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) n
#  define ARX_TUPLE_ELEM_15_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) o
 
#  define ARX_TUPLE_ELEM_16_0(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) a
#  define ARX_TUPLE_ELEM_16_1(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) b
#  define ARX_TUPLE_ELEM_16_2(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) c
#  define ARX_TUPLE_ELEM_16_3(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) d
#  define ARX_TUPLE_ELEM_16_4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) e
#  define ARX_TUPLE_ELEM_16_5(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) f
#  define ARX_TUPLE_ELEM_16_6(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) g
#  define ARX_TUPLE_ELEM_16_7(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) h
#  define ARX_TUPLE_ELEM_16_8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) i
#  define ARX_TUPLE_ELEM_16_9(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) j
#  define ARX_TUPLE_ELEM_16_10(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) k
#  define ARX_TUPLE_ELEM_16_11(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) l
#  define ARX_TUPLE_ELEM_16_12(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) m
#  define ARX_TUPLE_ELEM_16_13(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) n
#  define ARX_TUPLE_ELEM_16_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) o
#  define ARX_TUPLE_ELEM_16_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) p
 
#  define ARX_TUPLE_ELEM_17_0(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) a
#  define ARX_TUPLE_ELEM_17_1(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) b
#  define ARX_TUPLE_ELEM_17_2(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) c
#  define ARX_TUPLE_ELEM_17_3(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) d
#  define ARX_TUPLE_ELEM_17_4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) e
#  define ARX_TUPLE_ELEM_17_5(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) f
#  define ARX_TUPLE_ELEM_17_6(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) g
#  define ARX_TUPLE_ELEM_17_7(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) h
#  define ARX_TUPLE_ELEM_17_8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) i
#  define ARX_TUPLE_ELEM_17_9(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) j
#  define ARX_TUPLE_ELEM_17_10(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) k
#  define ARX_TUPLE_ELEM_17_11(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) l
#  define ARX_TUPLE_ELEM_17_12(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) m
#  define ARX_TUPLE_ELEM_17_13(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) n
#  define ARX_TUPLE_ELEM_17_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) o
#  define ARX_TUPLE_ELEM_17_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) p
#  define ARX_TUPLE_ELEM_17_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) q
 
#  define ARX_TUPLE_ELEM_18_0(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) a
#  define ARX_TUPLE_ELEM_18_1(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) b
#  define ARX_TUPLE_ELEM_18_2(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) c
#  define ARX_TUPLE_ELEM_18_3(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) d
#  define ARX_TUPLE_ELEM_18_4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) e
#  define ARX_TUPLE_ELEM_18_5(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) f
#  define ARX_TUPLE_ELEM_18_6(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) g
#  define ARX_TUPLE_ELEM_18_7(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) h
#  define ARX_TUPLE_ELEM_18_8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) i
#  define ARX_TUPLE_ELEM_18_9(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) j
#  define ARX_TUPLE_ELEM_18_10(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) k
#  define ARX_TUPLE_ELEM_18_11(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) l
#  define ARX_TUPLE_ELEM_18_12(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) m
#  define ARX_TUPLE_ELEM_18_13(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) n
#  define ARX_TUPLE_ELEM_18_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) o
#  define ARX_TUPLE_ELEM_18_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) p
#  define ARX_TUPLE_ELEM_18_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) q
#  define ARX_TUPLE_ELEM_18_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) r
 
#  define ARX_TUPLE_ELEM_19_0(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) a
#  define ARX_TUPLE_ELEM_19_1(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) b
#  define ARX_TUPLE_ELEM_19_2(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) c
#  define ARX_TUPLE_ELEM_19_3(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) d
#  define ARX_TUPLE_ELEM_19_4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) e
#  define ARX_TUPLE_ELEM_19_5(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) f
#  define ARX_TUPLE_ELEM_19_6(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) g
#  define ARX_TUPLE_ELEM_19_7(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) h
#  define ARX_TUPLE_ELEM_19_8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) i
#  define ARX_TUPLE_ELEM_19_9(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) j
#  define ARX_TUPLE_ELEM_19_10(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) k
#  define ARX_TUPLE_ELEM_19_11(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) l
#  define ARX_TUPLE_ELEM_19_12(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) m
#  define ARX_TUPLE_ELEM_19_13(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) n
#  define ARX_TUPLE_ELEM_19_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) o
#  define ARX_TUPLE_ELEM_19_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) p
#  define ARX_TUPLE_ELEM_19_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) q
#  define ARX_TUPLE_ELEM_19_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) r
#  define ARX_TUPLE_ELEM_19_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) s
 
#  define ARX_TUPLE_ELEM_20_0(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) a
#  define ARX_TUPLE_ELEM_20_1(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) b
#  define ARX_TUPLE_ELEM_20_2(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) c
#  define ARX_TUPLE_ELEM_20_3(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) d
#  define ARX_TUPLE_ELEM_20_4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) e
#  define ARX_TUPLE_ELEM_20_5(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) f
#  define ARX_TUPLE_ELEM_20_6(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) g
#  define ARX_TUPLE_ELEM_20_7(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) h
#  define ARX_TUPLE_ELEM_20_8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) i
#  define ARX_TUPLE_ELEM_20_9(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) j
#  define ARX_TUPLE_ELEM_20_10(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) k
#  define ARX_TUPLE_ELEM_20_11(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) l
#  define ARX_TUPLE_ELEM_20_12(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) m
#  define ARX_TUPLE_ELEM_20_13(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) n
#  define ARX_TUPLE_ELEM_20_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) o
#  define ARX_TUPLE_ELEM_20_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) p
#  define ARX_TUPLE_ELEM_20_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) q
#  define ARX_TUPLE_ELEM_20_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) r
#  define ARX_TUPLE_ELEM_20_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) s
#  define ARX_TUPLE_ELEM_20_19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) t
 
#  define ARX_TUPLE_ELEM_21_0(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) a
#  define ARX_TUPLE_ELEM_21_1(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) b
#  define ARX_TUPLE_ELEM_21_2(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) c
#  define ARX_TUPLE_ELEM_21_3(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) d
#  define ARX_TUPLE_ELEM_21_4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) e
#  define ARX_TUPLE_ELEM_21_5(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) f
#  define ARX_TUPLE_ELEM_21_6(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) g
#  define ARX_TUPLE_ELEM_21_7(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) h
#  define ARX_TUPLE_ELEM_21_8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) i
#  define ARX_TUPLE_ELEM_21_9(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) j
#  define ARX_TUPLE_ELEM_21_10(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) k
#  define ARX_TUPLE_ELEM_21_11(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) l
#  define ARX_TUPLE_ELEM_21_12(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) m
#  define ARX_TUPLE_ELEM_21_13(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) n
#  define ARX_TUPLE_ELEM_21_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) o
#  define ARX_TUPLE_ELEM_21_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) p
#  define ARX_TUPLE_ELEM_21_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) q
#  define ARX_TUPLE_ELEM_21_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) r
#  define ARX_TUPLE_ELEM_21_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) s
#  define ARX_TUPLE_ELEM_21_19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) t
#  define ARX_TUPLE_ELEM_21_20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) u
 
#  define ARX_TUPLE_ELEM_22_0(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) a
#  define ARX_TUPLE_ELEM_22_1(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) b
#  define ARX_TUPLE_ELEM_22_2(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) c
#  define ARX_TUPLE_ELEM_22_3(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) d
#  define ARX_TUPLE_ELEM_22_4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) e
#  define ARX_TUPLE_ELEM_22_5(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) f
#  define ARX_TUPLE_ELEM_22_6(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) g
#  define ARX_TUPLE_ELEM_22_7(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) h
#  define ARX_TUPLE_ELEM_22_8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) i
#  define ARX_TUPLE_ELEM_22_9(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) j
#  define ARX_TUPLE_ELEM_22_10(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) k
#  define ARX_TUPLE_ELEM_22_11(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) l
#  define ARX_TUPLE_ELEM_22_12(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) m
#  define ARX_TUPLE_ELEM_22_13(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) n
#  define ARX_TUPLE_ELEM_22_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) o
#  define ARX_TUPLE_ELEM_22_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) p
#  define ARX_TUPLE_ELEM_22_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) q
#  define ARX_TUPLE_ELEM_22_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) r
#  define ARX_TUPLE_ELEM_22_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) s
#  define ARX_TUPLE_ELEM_22_19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) t
#  define ARX_TUPLE_ELEM_22_20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) u
#  define ARX_TUPLE_ELEM_22_21(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) v
 
#  define ARX_TUPLE_ELEM_23_0(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) a
#  define ARX_TUPLE_ELEM_23_1(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) b
#  define ARX_TUPLE_ELEM_23_2(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) c
#  define ARX_TUPLE_ELEM_23_3(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) d
#  define ARX_TUPLE_ELEM_23_4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) e
#  define ARX_TUPLE_ELEM_23_5(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) f
#  define ARX_TUPLE_ELEM_23_6(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) g
#  define ARX_TUPLE_ELEM_23_7(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) h
#  define ARX_TUPLE_ELEM_23_8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) i
#  define ARX_TUPLE_ELEM_23_9(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) j
#  define ARX_TUPLE_ELEM_23_10(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) k
#  define ARX_TUPLE_ELEM_23_11(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) l
#  define ARX_TUPLE_ELEM_23_12(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) m
#  define ARX_TUPLE_ELEM_23_13(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) n
#  define ARX_TUPLE_ELEM_23_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) o
#  define ARX_TUPLE_ELEM_23_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) p
#  define ARX_TUPLE_ELEM_23_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) q
#  define ARX_TUPLE_ELEM_23_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) r
#  define ARX_TUPLE_ELEM_23_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) s
#  define ARX_TUPLE_ELEM_23_19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) t
#  define ARX_TUPLE_ELEM_23_20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) u
#  define ARX_TUPLE_ELEM_23_21(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) v
#  define ARX_TUPLE_ELEM_23_22(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) w
 
#  define ARX_TUPLE_ELEM_24_0(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) a
#  define ARX_TUPLE_ELEM_24_1(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) b
#  define ARX_TUPLE_ELEM_24_2(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) c
#  define ARX_TUPLE_ELEM_24_3(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) d
#  define ARX_TUPLE_ELEM_24_4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) e
#  define ARX_TUPLE_ELEM_24_5(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) f
#  define ARX_TUPLE_ELEM_24_6(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) g
#  define ARX_TUPLE_ELEM_24_7(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) h
#  define ARX_TUPLE_ELEM_24_8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) i
#  define ARX_TUPLE_ELEM_24_9(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) j
#  define ARX_TUPLE_ELEM_24_10(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) k
#  define ARX_TUPLE_ELEM_24_11(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) l
#  define ARX_TUPLE_ELEM_24_12(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) m
#  define ARX_TUPLE_ELEM_24_13(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) n
#  define ARX_TUPLE_ELEM_24_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) o
#  define ARX_TUPLE_ELEM_24_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) p
#  define ARX_TUPLE_ELEM_24_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) q
#  define ARX_TUPLE_ELEM_24_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) r
#  define ARX_TUPLE_ELEM_24_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) s
#  define ARX_TUPLE_ELEM_24_19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) t
#  define ARX_TUPLE_ELEM_24_20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) u
#  define ARX_TUPLE_ELEM_24_21(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) v
#  define ARX_TUPLE_ELEM_24_22(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) w
#  define ARX_TUPLE_ELEM_24_23(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) x
 
#  define ARX_TUPLE_ELEM_25_0(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) a
#  define ARX_TUPLE_ELEM_25_1(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) b
#  define ARX_TUPLE_ELEM_25_2(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) c
#  define ARX_TUPLE_ELEM_25_3(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) d
#  define ARX_TUPLE_ELEM_25_4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) e
#  define ARX_TUPLE_ELEM_25_5(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) f
#  define ARX_TUPLE_ELEM_25_6(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) g
#  define ARX_TUPLE_ELEM_25_7(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) h
#  define ARX_TUPLE_ELEM_25_8(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) i
#  define ARX_TUPLE_ELEM_25_9(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) j
#  define ARX_TUPLE_ELEM_25_10(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) k
#  define ARX_TUPLE_ELEM_25_11(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) l
#  define ARX_TUPLE_ELEM_25_12(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) m
#  define ARX_TUPLE_ELEM_25_13(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) n
#  define ARX_TUPLE_ELEM_25_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) o
#  define ARX_TUPLE_ELEM_25_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) p
#  define ARX_TUPLE_ELEM_25_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) q
#  define ARX_TUPLE_ELEM_25_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) r
#  define ARX_TUPLE_ELEM_25_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) s
#  define ARX_TUPLE_ELEM_25_19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) t
#  define ARX_TUPLE_ELEM_25_20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) u
#  define ARX_TUPLE_ELEM_25_21(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) v
#  define ARX_TUPLE_ELEM_25_22(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) w
#  define ARX_TUPLE_ELEM_25_23(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) x
#  define ARX_TUPLE_ELEM_25_24(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) y

#endif // ARX_USE_BOOST

#define ARX_EMPTY_IT(ARG) ARX_EMPTY
#define ARX_IDENTITY(ARG) ARG
#define ARX_PAREN_IT(ARG) (ARG)
#define ARX_BRACK_IT(ARG) [ARG]
#define ARX_ABRACK_IT(ARG) <ARG>


/**
 * Joins
 */
#define ARX_JOIN_2(a, b) ARX_JOIN(a, b)
#define ARX_JOIN_3(a, b, c) ARX_JOIN(a, ARX_JOIN(b, c))
#define ARX_JOIN_4(a, b, c, d) ARX_JOIN(a, ARX_JOIN_3(b, c, d))
#define ARX_JOIN_5(a, b, c, d, e) ARX_JOIN(a, ARX_JOIN_4(b, c, d, e))
#define ARX_JOIN_6(a, b, c, d, e, f) ARX_JOIN(a, ARX_JOIN_5(b, c, d, e, f))
#define ARX_JOIN_7(a, b, c, d, e, f, g) ARX_JOIN(a, ARX_JOIN_6(b, c, d, e, f, g))


/**
 * Array of indexes
 */
#define ARX_INDEX_ARRAY(SIZE) ARX_INDEX_ARRAY_I(SIZE)
#define ARX_INDEX_ARRAY_I(SIZE) ARX_INDEX_ARRAY_II(ARX_INDEX_ARRAY_ ## SIZE ())
#define ARX_INDEX_ARRAY_II(res) res

#define ARX_INDEX_ARRAY_0() (0, ())
#define ARX_INDEX_ARRAY_1() (1, (0))
#define ARX_INDEX_ARRAY_2() (2, (0, 1))
#define ARX_INDEX_ARRAY_3() (3, (0, 1, 2))
#define ARX_INDEX_ARRAY_4() (4, (0, 1, 2, 3))
#define ARX_INDEX_ARRAY_5() (5, (0, 1, 2, 3, 4))
#define ARX_INDEX_ARRAY_6() (6, (0, 1, 2, 3, 4, 5))
#define ARX_INDEX_ARRAY_7() (7, (0, 1, 2, 3, 4, 5, 6))
#define ARX_INDEX_ARRAY_8() (8, (0, 1, 2, 3, 4, 5, 6, 7))
#define ARX_INDEX_ARRAY_9() (9, (0, 1, 2, 3, 4, 5, 6, 7, 8))
#define ARX_INDEX_ARRAY_10() (10, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9))
#define ARX_INDEX_ARRAY_11() (11, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10))
#define ARX_INDEX_ARRAY_12() (12, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11))
#define ARX_INDEX_ARRAY_13() (13, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12))
#define ARX_INDEX_ARRAY_14() (14, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13))
#define ARX_INDEX_ARRAY_15() (15, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14))
#define ARX_INDEX_ARRAY_16() (16, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15))
#define ARX_INDEX_ARRAY_17() (17, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16))
#define ARX_INDEX_ARRAY_18() (18, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17))
#define ARX_INDEX_ARRAY_19() (19, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18))
#define ARX_INDEX_ARRAY_20() (20, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19))
#define ARX_INDEX_ARRAY_21() (21, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20))
#define ARX_INDEX_ARRAY_22() (22, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21))
#define ARX_INDEX_ARRAY_23() (23, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22))
#define ARX_INDEX_ARRAY_24() (24, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23))
#define ARX_INDEX_ARRAY_25() (25, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24))


/**
 * Array foreach macro, max depth = 3
 */
#if 0
#  define ARX_ARRAY_FOREACH(ARRAY, M, ARG)
#endif

#define ARX_ARRAY_FOREACH ARX_JOIN(ARX_ARRAY_FOREACH_, ARX_AUTO_REC(ARX_ARRAY_FOREACH_P, 4))

#define ARX_ARRAY_FOREACH_P(n) ARX_JOIN(ARX_ARRAY_FOREACH_CHECK_, ARX_ARRAY_FOREACH_ ## n((1, (0)), ARX_NIL ARX_TUPLE_EAT_2, ARX_NIL))

#define ARX_ARRAY_FOREACH_CHECK_ARX_NIL 1
#define ARX_ARRAY_FOREACH_CHECK_ARX_ARRAY_FOREACH_1(ARRAY, M, ARG) 0
#define ARX_ARRAY_FOREACH_CHECK_ARX_ARRAY_FOREACH_2(ARRAY, M, ARG) 0
#define ARX_ARRAY_FOREACH_CHECK_ARX_ARRAY_FOREACH_3(ARRAY, M, ARG) 0

#define ARX_ARRAY_FOREACH_1(ARRAY, M, ARG) ARX_ARRAY_FOREACH_1_OO(ARX_ARRAY_SIZE(ARRAY), ARX_ARRAY_REVERSE(ARRAY), M, ARG)
#define ARX_ARRAY_FOREACH_1_OO(SIZE, ARRAY, M, ARG) ARX_ARRAY_FOREACH_1_I(SIZE, ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_I(SIZE, ARRAY, M, ARG) ARX_ARRAY_FOREACH_1_II(ARX_ARRAY_FOREACH_1_ ## SIZE (ARRAY, M, ARG))
#define ARX_ARRAY_FOREACH_1_II(res) res
#define ARX_ARRAY_FOREACH_1_0(ARRAY, M, ARG) 
#define ARX_ARRAY_FOREACH_1_1(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(0, ARRAY), ARG)
#define ARX_ARRAY_FOREACH_1_2(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(1, ARRAY), ARG)ARX_ARRAY_FOREACH_1_1(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_3(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(2, ARRAY), ARG)ARX_ARRAY_FOREACH_1_2(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_4(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(3, ARRAY), ARG)ARX_ARRAY_FOREACH_1_3(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_5(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(4, ARRAY), ARG)ARX_ARRAY_FOREACH_1_4(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_6(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(5, ARRAY), ARG)ARX_ARRAY_FOREACH_1_5(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_7(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(6, ARRAY), ARG)ARX_ARRAY_FOREACH_1_6(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_8(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(7, ARRAY), ARG)ARX_ARRAY_FOREACH_1_7(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_9(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(8, ARRAY), ARG)ARX_ARRAY_FOREACH_1_8(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_10(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(9, ARRAY), ARG)ARX_ARRAY_FOREACH_1_9(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_11(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(10, ARRAY), ARG)ARX_ARRAY_FOREACH_1_10(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_12(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(11, ARRAY), ARG)ARX_ARRAY_FOREACH_1_11(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_13(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(12, ARRAY), ARG)ARX_ARRAY_FOREACH_1_12(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_14(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(13, ARRAY), ARG)ARX_ARRAY_FOREACH_1_13(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_15(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(14, ARRAY), ARG)ARX_ARRAY_FOREACH_1_14(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_16(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(15, ARRAY), ARG)ARX_ARRAY_FOREACH_1_15(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_17(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(16, ARRAY), ARG)ARX_ARRAY_FOREACH_1_16(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_18(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(17, ARRAY), ARG)ARX_ARRAY_FOREACH_1_17(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_19(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(18, ARRAY), ARG)ARX_ARRAY_FOREACH_1_18(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_20(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(19, ARRAY), ARG)ARX_ARRAY_FOREACH_1_19(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_21(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(20, ARRAY), ARG)ARX_ARRAY_FOREACH_1_20(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_22(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(21, ARRAY), ARG)ARX_ARRAY_FOREACH_1_21(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_23(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(22, ARRAY), ARG)ARX_ARRAY_FOREACH_1_22(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_24(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(23, ARRAY), ARG)ARX_ARRAY_FOREACH_1_23(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_1_25(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(24, ARRAY), ARG)ARX_ARRAY_FOREACH_1_24(ARRAY, M, ARG)

#define ARX_ARRAY_FOREACH_2(ARRAY, M, ARG) ARX_ARRAY_FOREACH_2_OO(ARX_ARRAY_SIZE(ARRAY), ARX_ARRAY_REVERSE(ARRAY), M, ARG)
#define ARX_ARRAY_FOREACH_2_OO(SIZE, ARRAY, M, ARG) ARX_ARRAY_FOREACH_2_I(SIZE, ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_I(SIZE, ARRAY, M, ARG) ARX_ARRAY_FOREACH_2_II(ARX_ARRAY_FOREACH_2_ ## SIZE (ARRAY, M, ARG))
#define ARX_ARRAY_FOREACH_2_II(res) res
#define ARX_ARRAY_FOREACH_2_0(ARRAY, M, ARG) 
#define ARX_ARRAY_FOREACH_2_1(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(0, ARRAY), ARG)
#define ARX_ARRAY_FOREACH_2_2(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(1, ARRAY), ARG)ARX_ARRAY_FOREACH_2_1(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_3(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(2, ARRAY), ARG)ARX_ARRAY_FOREACH_2_2(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_4(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(3, ARRAY), ARG)ARX_ARRAY_FOREACH_2_3(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_5(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(4, ARRAY), ARG)ARX_ARRAY_FOREACH_2_4(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_6(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(5, ARRAY), ARG)ARX_ARRAY_FOREACH_2_5(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_7(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(6, ARRAY), ARG)ARX_ARRAY_FOREACH_2_6(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_8(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(7, ARRAY), ARG)ARX_ARRAY_FOREACH_2_7(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_9(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(8, ARRAY), ARG)ARX_ARRAY_FOREACH_2_8(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_10(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(9, ARRAY), ARG)ARX_ARRAY_FOREACH_2_9(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_11(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(10, ARRAY), ARG)ARX_ARRAY_FOREACH_2_10(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_12(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(11, ARRAY), ARG)ARX_ARRAY_FOREACH_2_11(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_13(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(12, ARRAY), ARG)ARX_ARRAY_FOREACH_2_12(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_14(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(13, ARRAY), ARG)ARX_ARRAY_FOREACH_2_13(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_15(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(14, ARRAY), ARG)ARX_ARRAY_FOREACH_2_14(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_16(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(15, ARRAY), ARG)ARX_ARRAY_FOREACH_2_15(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_17(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(16, ARRAY), ARG)ARX_ARRAY_FOREACH_2_16(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_18(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(17, ARRAY), ARG)ARX_ARRAY_FOREACH_2_17(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_19(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(18, ARRAY), ARG)ARX_ARRAY_FOREACH_2_18(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_20(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(19, ARRAY), ARG)ARX_ARRAY_FOREACH_2_19(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_21(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(20, ARRAY), ARG)ARX_ARRAY_FOREACH_2_20(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_22(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(21, ARRAY), ARG)ARX_ARRAY_FOREACH_2_21(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_23(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(22, ARRAY), ARG)ARX_ARRAY_FOREACH_2_22(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_24(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(23, ARRAY), ARG)ARX_ARRAY_FOREACH_2_23(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_2_25(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(24, ARRAY), ARG)ARX_ARRAY_FOREACH_2_24(ARRAY, M, ARG)

#define ARX_ARRAY_FOREACH_3(ARRAY, M, ARG) ARX_ARRAY_FOREACH_3_OO(ARX_ARRAY_SIZE(ARRAY), ARX_ARRAY_REVERSE(ARRAY), M, ARG)
#define ARX_ARRAY_FOREACH_3_OO(SIZE, ARRAY, M, ARG) ARX_ARRAY_FOREACH_3_I(SIZE, ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_I(SIZE, ARRAY, M, ARG) ARX_ARRAY_FOREACH_3_II(ARX_ARRAY_FOREACH_3_ ## SIZE (ARRAY, M, ARG))
#define ARX_ARRAY_FOREACH_3_II(res) res
#define ARX_ARRAY_FOREACH_3_0(ARRAY, M, ARG) 
#define ARX_ARRAY_FOREACH_3_1(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(0, ARRAY), ARG)
#define ARX_ARRAY_FOREACH_3_2(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(1, ARRAY), ARG)ARX_ARRAY_FOREACH_3_1(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_3(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(2, ARRAY), ARG)ARX_ARRAY_FOREACH_3_2(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_4(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(3, ARRAY), ARG)ARX_ARRAY_FOREACH_3_3(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_5(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(4, ARRAY), ARG)ARX_ARRAY_FOREACH_3_4(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_6(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(5, ARRAY), ARG)ARX_ARRAY_FOREACH_3_5(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_7(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(6, ARRAY), ARG)ARX_ARRAY_FOREACH_3_6(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_8(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(7, ARRAY), ARG)ARX_ARRAY_FOREACH_3_7(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_9(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(8, ARRAY), ARG)ARX_ARRAY_FOREACH_3_8(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_10(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(9, ARRAY), ARG)ARX_ARRAY_FOREACH_3_9(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_11(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(10, ARRAY), ARG)ARX_ARRAY_FOREACH_3_10(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_12(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(11, ARRAY), ARG)ARX_ARRAY_FOREACH_3_11(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_13(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(12, ARRAY), ARG)ARX_ARRAY_FOREACH_3_12(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_14(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(13, ARRAY), ARG)ARX_ARRAY_FOREACH_3_13(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_15(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(14, ARRAY), ARG)ARX_ARRAY_FOREACH_3_14(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_16(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(15, ARRAY), ARG)ARX_ARRAY_FOREACH_3_15(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_17(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(16, ARRAY), ARG)ARX_ARRAY_FOREACH_3_16(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_18(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(17, ARRAY), ARG)ARX_ARRAY_FOREACH_3_17(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_19(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(18, ARRAY), ARG)ARX_ARRAY_FOREACH_3_18(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_20(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(19, ARRAY), ARG)ARX_ARRAY_FOREACH_3_19(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_21(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(20, ARRAY), ARG)ARX_ARRAY_FOREACH_3_20(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_22(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(21, ARRAY), ARG)ARX_ARRAY_FOREACH_3_21(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_23(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(22, ARRAY), ARG)ARX_ARRAY_FOREACH_3_22(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_24(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(23, ARRAY), ARG)ARX_ARRAY_FOREACH_3_23(ARRAY, M, ARG)
#define ARX_ARRAY_FOREACH_3_25(ARRAY, M, ARG) M(ARX_ARRAY_ELEM(24, ARRAY), ARG)ARX_ARRAY_FOREACH_3_24(ARRAY, M, ARG)

#define ARX_ARRAY_FOREACH_4(a, m, d) OMG_TEH_DRAMA /* error */


/**
 * @param PRED predicate of the form PRED(ELEM, ARG) which returns 0 or 1
 * @returns 1 if there exists an element in ARRAY that satisfies the given predicate PRED, 0 otherwise
 */
#define ARX_ARRAY_EXISTS(ARRAY, PRED, ARG)                                      \
  ARX_ARRAY_EXISTS_III(ARX_ARRAY_FOREACH(ARRAY, ARX_ARRAY_EXISTS_I, (2, (PRED, ARG)))(ARX_NIL))

#define ARX_ARRAY_EXISTS_I(ELEM, ARGS) ARX_ARRAY_EXISTS_II(ELEM, ARX_ARRAY_ELEM(0, ARGS), ARX_ARRAY_ELEM(1, ARGS))
#define ARX_ARRAY_EXISTS_II(ELEM, PRED, ARG) (PRED(ELEM, ARG))

#define ARX_ARRAY_EXISTS_III(res) ARX_ARRAY_EXISTS_IIII(ARX_ARRAY_EXISTS_F_0 res)
#define ARX_ARRAY_EXISTS_IIII(res) res

#define ARX_ARRAY_EXISTS_HELPER_ARX_NIL
#define ARX_ARRAY_EXISTS_F_0(V) ARX_IF(ARX_IS_EMPTY(ARX_JOIN(ARX_ARRAY_EXISTS_HELPER_, V)), 0, ARX_IF(V, ARX_ARRAY_EXISTS_T_1, ARX_ARRAY_EXISTS_F_1))
#define ARX_ARRAY_EXISTS_F_1(V) ARX_IF(ARX_IS_EMPTY(ARX_JOIN(ARX_ARRAY_EXISTS_HELPER_, V)), 0, ARX_IF(V, ARX_ARRAY_EXISTS_T_0, ARX_ARRAY_EXISTS_F_0))
#define ARX_ARRAY_EXISTS_T_0(V) ARX_IF(ARX_IS_EMPTY(ARX_JOIN(ARX_ARRAY_EXISTS_HELPER_, V)), 1, ARX_ARRAY_EXISTS_T_1)
#define ARX_ARRAY_EXISTS_T_1(V) ARX_IF(ARX_IS_EMPTY(ARX_JOIN(ARX_ARRAY_EXISTS_HELPER_, V)), 1, ARX_ARRAY_EXISTS_T_0)


#endif


