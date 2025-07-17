--
-- PostgreSQL database dump
--

-- Dumped from database version 17.5
-- Dumped by pg_dump version 17.5

-- Started on 2025-07-11 19:09:04

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET transaction_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- TOC entry 218 (class 1259 OID 16664)
-- Name: ministries; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.ministries (
    ministry_id integer NOT NULL,
    ministry_name text NOT NULL
);


ALTER TABLE public.ministries OWNER TO postgres;

--
-- TOC entry 217 (class 1259 OID 16663)
-- Name: ministries_ministry_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.ministries_ministry_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.ministries_ministry_id_seq OWNER TO postgres;

--
-- TOC entry 4931 (class 0 OID 0)
-- Dependencies: 217
-- Name: ministries_ministry_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.ministries_ministry_id_seq OWNED BY public.ministries.ministry_id;


--
-- TOC entry 220 (class 1259 OID 16673)
-- Name: offices; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.offices (
    office_id integer NOT NULL,
    office_name text NOT NULL,
    ministry_id integer
);


ALTER TABLE public.offices OWNER TO postgres;

--
-- TOC entry 219 (class 1259 OID 16672)
-- Name: offices_office_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.offices_office_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.offices_office_id_seq OWNER TO postgres;

--
-- TOC entry 4932 (class 0 OID 0)
-- Dependencies: 219
-- Name: offices_office_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.offices_office_id_seq OWNED BY public.offices.office_id;


--
-- TOC entry 224 (class 1259 OID 16701)
-- Name: required_documents; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.required_documents (
    document_id integer NOT NULL,
    service_id integer,
    document_text text
);


ALTER TABLE public.required_documents OWNER TO postgres;

--
-- TOC entry 223 (class 1259 OID 16700)
-- Name: required_documents_document_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.required_documents_document_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.required_documents_document_id_seq OWNER TO postgres;

--
-- TOC entry 4933 (class 0 OID 0)
-- Dependencies: 223
-- Name: required_documents_document_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.required_documents_document_id_seq OWNED BY public.required_documents.document_id;


--
-- TOC entry 222 (class 1259 OID 16687)
-- Name: services; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.services (
    service_id integer NOT NULL,
    office_id integer,
    service_no integer,
    service_name text,
    time_taken text,
    charge text,
    contact_section text,
    grievance_section text,
    monitoring_officer text,
    responsible_officer text,
    remarks text
);


ALTER TABLE public.services OWNER TO postgres;

--
-- TOC entry 221 (class 1259 OID 16686)
-- Name: services_service_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.services_service_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.services_service_id_seq OWNER TO postgres;

--
-- TOC entry 4934 (class 0 OID 0)
-- Dependencies: 221
-- Name: services_service_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.services_service_id_seq OWNED BY public.services.service_id;


--
-- TOC entry 4757 (class 2604 OID 16667)
-- Name: ministries ministry_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ministries ALTER COLUMN ministry_id SET DEFAULT nextval('public.ministries_ministry_id_seq'::regclass);


--
-- TOC entry 4758 (class 2604 OID 16676)
-- Name: offices office_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.offices ALTER COLUMN office_id SET DEFAULT nextval('public.offices_office_id_seq'::regclass);


--
-- TOC entry 4760 (class 2604 OID 16704)
-- Name: required_documents document_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.required_documents ALTER COLUMN document_id SET DEFAULT nextval('public.required_documents_document_id_seq'::regclass);


--
-- TOC entry 4759 (class 2604 OID 16690)
-- Name: services service_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.services ALTER COLUMN service_id SET DEFAULT nextval('public.services_service_id_seq'::regclass);


--
-- TOC entry 4919 (class 0 OID 16664)
-- Dependencies: 218
-- Data for Name: ministries; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.ministries (ministry_id, ministry_name) FROM stdin;
1	Ministry of Culture, Tourism and Civil Aviation
2	Ministry of Foreign Affairs
3	Home Affairs
4	Ministry of Labour, Employment and Social Security
5	Ministry of Land Management, Cooperatives and Poverty Alleviation
6	Ministry of Agriculture and Livestock Development
7	Ministry of Physical Infrastructure and Transport
\.


--
-- TOC entry 4921 (class 0 OID 16673)
-- Dependencies: 220
-- Data for Name: offices; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.offices (office_id, office_name, ministry_id) FROM stdin;
1	National Museum	1
2	Department of Passport	2
3	Department of Immigration	3
4	Department of Labour and Occupational Safety	4
5	Land Reform Office	5
6	Land Management Office	5
7	Department of Agriculture	6
8	Department of Transport Management	7
9	District Administration Office (DAO)	3
10	Ward Office	3
11	District Court	3
12	Mobile Enrollment Units	3
\.


--
-- TOC entry 4925 (class 0 OID 16701)
-- Dependencies: 224
-- Data for Name: required_documents; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.required_documents (document_id, service_id, document_text) FROM stdin;
1	1	Students must bring student ID card for discount
2	2	-
3	3	-
4	4	-
5	5	-
6	6	-
7	7	-
8	8	-
9	9	-
10	10	-
11	11	-
12	12	-
13	13	-
14	14	-
15	15	-
16	16	-
17	17	-
18	18	-
19	19	Filled online form, Nepali Citizenship Certificate, National ID Card, Old passport (if available), Minor ID card (for minors)
20	20	Filled online form, Citizenship/Minor ID (certified), National ID Card (if mandatory), Old passport (if available), Other required documents (see details in respective services)
21	21	Minor ID card, Citizenship certificate
22	22	Application form, SMS/Message
23	23	National ID Card document, SMS/Message (if received)
24	24	Citizenship certificate, National ID Card
25	25	Online form, Relevant documents and information sources
26	26	Relevant documents as per nature of service
27	27	Passport, Visa, Identity certificate, Other relevant documents
28	28	Filled online form, Revenue receipt
29	29	Online form, Citizenship certificate
30	30	Online form, Citizenship certificate, Old passport, Other relevant documents
31	31	Online form, Citizenship certificate, Revenue receipt, Old passport
32	32	Home Ministry's letter, Copy of Home Ministry's bank draft, Other required documents
33	33	Letter from relevant constitutional body/ministry/federal court, Copy of bank draft, Nepali citizenship certificate, Online form
34	34	Relevant personality and documents
35	35	Certificate
36	36	Certificate
37	37	Relevant documents
38	38	—
39	39	Receipt of Online Application Form,\nLicense issued by the Tourism Industry Division, Ministry of Culture, Tourism and Civil Aviation to operate trekking business,\nLicense issued by Nepal Rastra Bank allowing exchanging foreign currencies,\nProgram Schedule of Trekking,\nRegistration certificate of Permanent Account Number,\nName list of trekkers,\nGuarantee letter of Agency,\nAgreement with the Agency,\nPhotocopy of voucher of payment of prescribed revenue (in foreign currency) Nepal Rastra Bank or Globel IME Bank,\nCash receipt submitted to Bank,\nTrekkers application form to be filled in by trekkers along with photo,\nDocuments relating to insurance of the foreign national going to trekking and Nepalese staff accompanying the trekkers,\nTax Clearance Certificate of the Trekking Agency,\nApplication form from Trekking Agency
40	40	Passport,\nReceipt of Online Application,\nTax clearance certificate in case of Transfer from Business visa to Tourist Visa,\nCopy of passport of guardian incase of minor below 10 years of Age
41	41	New passport,\nCopy of the new passport,\nIf the visa is not registered, proof of arrival and application,\nOne photograph,\nPolice report
42	42	Old passport,\nNew passport,\nCopy of both passports,\nOne photograph
43	43	Passport,\nApplication,\nTicket of Visit,\nDeparture stamp of last departure point,\nBoarding pass of visit,\nPassenger manifesto
44	44	Passport and its photocopy,\nApplication,\nBoarding Pass of departure, air ticket or document related to flight,\nArrival stamp of next point of entry
45	45	Receipt of Online Application,\nCopy of company registration,\nCopy of PAN/VAT certificate,\nCopy of the share certificate,\nCopy of Industry registration certificate,\nCopy of passport and recent Nepalese visa,\nBio Data of Investor,\nTax clearance certificate for renewal
46	46	Receipt of Online application Admission receipt from College / University,\nRecommendation from Ministry of Education,\nBank Statement (USD 3000 per year and in case of dependent USD 2000 per person per year) or receipt of exchange of equivalent currency or Nepalese currency,\nCopy of passport and recent Nepalese visa,\nWork permit for Teacher / Workers,\nAppointment letter / contract paper for teachers
116	116	Documents required as per the subject matter
117	117	Application, Technical Specification, Test Report, Catalog, Drawing Estimate
118	118	Letter requesting opinion/feedback with proposed constitution of the association/organization
119	119	As per Factory and Workshop Operation Guidelines, 2074
120	120	Documents required as per the subject matter
121	121	Request letter/application
122	122	Request letter/application
123	123	Application, Technical Specification, Test Report (007 and 10), Catalog, Dealership Agreement, Company Registration and Other Related Documents
47	47	Receipt of Online application,\nBank statement of USD 20,000,\nCertificate having regular income at least USD 20,000 per year,\nCharacter Certificate,\nNo Objection letter from concerned country of Embassy,\nHealth certificate issued by TU Teaching Hospital,\nThe evidence of completing 60 years (or) proof of retirement,\nCopy of valid passport and recent Nepalese visa,\nFor renewal Expenditure Statement of USD 20,000
48	48	Receipt of Online application,\nCopy of company registration certificate,\nCopy PAN/VAT certificate,\nCopy of the share certificate,\nCopy of industry registration certificate,\nRecommendation from Dept. of Industry or Commerce,\nCopy of passport and recent Nepalese visa,\nTax clearance certificate for renewal
49	49	Passport,\nCopy of relationship certificate or recommendation from Nepalese Embassy or Mission,\nHealth certificate,\nCopy of passport and recent Nepalese visa,\nFor Non residential Nepalese student visa, recommendation letter of the educational institute in Nepal
50	50	Receipt of Online Application,\nWork permit/labour permit,\nRecommendation from concerned Ministry,\nCopy of passport and recent Nepalese visa,\nNT (RELATION) VISA - Application from Nepalese relative, copy of relationship certificate and citizenship certificate of Nepalese relative (for Nepalese Origin) Or Nepali Spouse's relatives and others Relationship Certificate from concerned Embassy / Mission,\nNT (Marriage Visa) : Marriage certificate and Citizenship certificate of Nepalese spouse,\nNT (Press) Visa : Press Representative Certificate and recommendation from Dept. of Information
51	51	Receipt of Online Application Form,\nLicense issued by the Tourism Industry Division, Ministry of Culture, Tourism and Civil Aviation to operate trekking business,\nLicense issued by Nepal Rastra Bank allowing exchanging foreign currencies,\nProgram Schedule of Trekking,\nRegistration certificate of Permanent Account Number,\nName list of trekkers,\nGuarantee letter of Agency,\nAgreement with the Agency,\nPhotocopy of voucher of payment of prescribed revenue (in foreign currency) Nepal Rastra Bank or Globel IME Bank,\nCash receipt submitted to Bank,\nTrekkers application form to be filled in by trekkers along with photo,\nDocuments relating to insurance of the foreign national going to trekking and Nepalese staff accompanying the trekkers,\nTax Clearance Certificate of the Trekking Agency,\nApplication form from Trekking Agency
52	52	Receipt of Online Application Form,\nLicense issued by the Tourism Industry Division, Ministry of Culture, Tourism and Civil Aviation to operate trekking business,\nLicense issued by Nepal Rastra Bank allowing exchanging foreign currencies,\nProgram Schedule of Trekking,\nRegistration certificate of Permanent Account Number,\nName list of trekkers,\nGuarantee letter of Agency,\nAgreement with the Agency,\nPhotocopy of voucher of payment of prescribed revenue (in foreign currency) Nepal Rastra Bank or Globel IME Bank,\nCash receipt submitted to Bank,\nTrekkers application form to be filled in by trekkers along with photo,\nDocuments relating to insurance of the foreign national going to trekking and Nepalese staff accompanying the trekkers,\nTax Clearance Certificate of the Trekking Agency,\nApplication form from Trekking Agency
53	53	Receipt of Online Application Form,\nLicense issued by the Tourism Industry Division, Ministry of Culture, Tourism and Civil Aviation to operate trekking business,\nLicense issued by Nepal Rastra Bank allowing exchanging foreign currencies,\nProgram Schedule of Trekking,\nRegistration certificate of Permanent Account Number,\nName list of trekkers,\nGuarantee letter of Agency,\nAgreement with the Agency,\nPhotocopy of voucher of payment of prescribed revenue (in foreign currency) Nepal Rastra Bank or Globel IME Bank,\nCash receipt submitted to Bank,\nTrekkers application form to be filled in by trekkers along with photo,\nDocuments relating to insurance of the foreign national going to trekking and Nepalese staff accompanying the trekkers,\nTax Clearance Certificate of the Trekking Agency,\nApplication form from Trekking Agency
54	54	Receipt of Online Application Form,\nLicense issued by the Tourism Industry Division, Ministry of Culture, Tourism and Civil Aviation to operate trekking business,\nLicense issued by Nepal Rastra Bank allowing exchanging foreign currencies,\nProgram Schedule of Trekking,\nRegistration certificate of Permanent Account Number,\nName list of trekkers,\nGuarantee letter of Agency,\nAgreement with the Agency,\nPhotocopy of voucher of payment of prescribed revenue (in foreign currency) Nepal Rastra Bank or Globel IME Bank,\nCash receipt submitted to Bank,\nTrekkers application form to be filled in by trekkers along with photo,\nDocuments relating to insurance of the foreign national going to trekking and Nepalese staff accompanying the trekkers,\nTax Clearance Certificate of the Trekking Agency,\nApplication form from Trekking Agency
55	55	Receipt of Online Application Form, License issued by the Tourism Industry Division, Ministry of Culture, Tourism and Civil Aviation to operate trekking business, License issued by Nepal Rastra Bank allowing exchanging foreign currencies, Program Schedule of Trekking, Registration certificate of Permanent Account Number, Name list of trekkers, Guarantee letter of Agency, Agreement with the Agency, Photocopy of voucher of payment of prescribed revenue (in foreign currency) Nepal Rastra Bank or Globel IME Bank, Cash receipt submitted to Bank, Trekkers application form to be filled in by trekkers along with photo, Documents relating to insurance of the foreign national going to trekking and Nepalese staff accompanying the trekkers, Tax Clearance Certificate of the Trekking Agency, Application form from Trekking Agency
56	56	Receipt of Online Application Form, License issued by the Tourism Industry Division, Ministry of Culture, Tourism and Civil Aviation to operate trekking business, License issued by Nepal Rastra Bank allowing exchanging foreign currencies, Program Schedule of Trekking, Registration certificate of Permanent Account Number, Name list of trekkers, Guarantee letter of Agency, Agreement with the Agency, Photocopy of voucher of payment of prescribed revenue (in foreign currency) Nepal Rastra Bank or Globel IME Bank, Cash receipt submitted to Bank, Trekkers application form to be filled in by trekkers along with photo, Documents relating to insurance of the foreign national going to trekking and Nepalese staff accompanying the trekkers, Tax Clearance Certificate of the Trekking Agency, Application form from Trekking Agency
57	57	Receipt of Online Application Form, License issued by the Tourism Industry Division, Ministry of Culture, Tourism and Civil Aviation to operate trekking business, License issued by Nepal Rastra Bank allowing exchanging foreign currencies, Program Schedule of Trekking, Registration certificate of Permanent Account Number, Name list of trekkers, Guarantee letter of Agency, Agreement with the Agency, Photocopy of voucher of payment of prescribed revenue (in foreign currency) Nepal Rastra Bank or Globel IME Bank, Cash receipt submitted to Bank, Trekkers application form to be filled in by trekkers along with photo, Documents relating to insurance of the foreign national going to trekking and Nepalese staff accompanying the trekkers, Tax Clearance Certificate of the Trekking Agency, Application form from Trekking Agency
58	58	Receipt of Online Application Form, License issued by the Tourism Industry Division, Ministry of Culture, Tourism and Civil Aviation to operate trekking business, License issued by Nepal Rastra Bank allowing exchanging foreign currencies, Program Schedule of Trekking, Registration certificate of Permanent Account Number, Name list of trekkers, Guarantee letter of Agency, Agreement with the Agency, Photocopy of voucher of payment of prescribed revenue (in foreign currency) Nepal Rastra Bank or Globel IME Bank, Cash receipt submitted to Bank, Trekkers application form to be filled in by trekkers along with photo, Documents relating to insurance of the foreign national going to trekking and Nepalese staff accompanying the trekkers, Tax Clearance Certificate of the Trekking Agency, Application form from Trekking Agency
59	59	Receipt of Online Application Form, License issued by the Tourism Industry Division, Ministry of Culture, Tourism and Civil Aviation to operate trekking business, License issued by Nepal Rastra Bank allowing exchanging foreign currencies, Program Schedule of Trekking, Registration certificate of Permanent Account Number, Name list of trekkers, Guarantee letter of Agency, Agreement with the Agency, Photocopy of voucher of payment of prescribed revenue (in foreign currency) Nepal Rastra Bank or Globel IME Bank, Cash receipt submitted to Bank, Trekkers application form to be filled in by trekkers along with photo, Documents relating to insurance of the foreign national going to trekking and Nepalese staff accompanying the trekkers, Tax Clearance Certificate of the Trekking Agency, Application form from Trekking Agency
60	60	Receipt of Online Application Form, License issued by the Tourism Industry Division, Ministry of Culture, Tourism and Civil Aviation to operate trekking business, License issued by Nepal Rastra Bank allowing exchanging foreign currencies, Program Schedule of Trekking, Registration certificate of Permanent Account Number, Name list of trekkers, Guarantee letter of Agency, Agreement with the Agency, Photocopy of voucher of payment of prescribed revenue (in foreign currency) Nepal Rastra Bank or Globel IME Bank, Cash receipt submitted to Bank, Trekkers application form to be filled in by trekkers along with photo, Documents relating to insurance of the foreign national going to trekking and Nepalese staff accompanying the trekkers, Tax Clearance Certificate of the Trekking Agency, Application form from Trekking Agency
61	61	Receipt of Online Application Form, License issued by the Tourism Industry Division, Ministry of Culture, Tourism and Civil Aviation to operate trekking business, License issued by Nepal Rastra Bank allowing exchanging foreign currencies, Program Schedule of Trekking, Registration certificate of Permanent Account Number, Name list of trekkers, Guarantee letter of Agency, Agreement with the Agency, Photocopy of voucher of payment of prescribed revenue (in foreign currency) Nepal Rastra Bank or Globel IME Bank, Cash receipt submitted to Bank, Trekkers application form to be filled in by trekkers along with photo, Documents relating to insurance of the foreign national going to trekking and Nepalese staff accompanying the trekkers, Tax Clearance Certificate of the Trekking Agency, Application form from Trekking Agency
62	62	Documents supporting the details mentioned in serial no. 2 and 3
63	63	Submission of communication letter, bill payment, and test certificate for new boiler operation
64	64	Application must be submitted within the end of each fiscal year
65	65	Companies wishing to obtain a labor supply license must submit an application to the office with the following documents:, Certified copy of company registration certificate, Certified copy of the company's memorandum and articles of association, Certified copy of permanent account number or VAT registration certificate, Tax clearance certificate as per prevailing law, Certified copy of citizenship certificate of company directors, Details of the work or service for which labor supply is required, Deposit or bank guarantee of NPR 1,500,000 as per Schedule-5, Other documents as specified by the office
66	66	To renew the obtained license, the company must submit an application to the office with the following details by the end of Ashwin each year:, Receipt of income statement of the previous fiscal year or tax clearance certificate of the year before that, Proof of renewal of bank guarantee, Labor audit report, Details of the number of workers supplied in the previous fiscal year, the institutions where they are employed, and the main employers, Details of wages, allowances, facilities provided to supplied workers, and contributions made to the Social Security Fund, Company address, name of directors, contact number, and details of any changes in company information
67	67	To inspect whether labor laws and regulations have been implemented in the establishments, Observe arrangements related to occupational safety and health and provide necessary instructions
68	68	If the dispute is not resolved through reconciliation within 15 days of submission to the employer, an application can be submitted to the office:, Application with signatures of concerned workers/employees, Other documents supporting the claim
69	69	If agreement is not reached through negotiation within 21 days of claim submission, an application can be submitted to the office
70	70	Inspect the factory as necessary and take required action
71	71	Carry out initial tasks related to labor administration, Prepare monthly, quarterly, and annual progress reports regularly, Record minutes of meetings between management and workers/employees
72	72	Carry out administrative tasks as required for daily operations
73	73	Acceptance details form (certified by the manager) original and 3 copies of Form No. 1, Documents filled as per Forms No. 2, 3, 4, 5, and 6, copy of the decision of the workers' meeting regarding trade union registration, copy of the statute, Receipt of Rs. 150 for new registration, Certified copy of citizenship certificate of office bearers of the executive committee
74	74	All required documents for trade union registration as per serial no. 11 (a, b, c, d), cash receipt of Rs. 792 for renewal, and audited income-expenditure statement (audit report) certified by a registered auditor, If submitted within 3 months before the expiry of the recognition period, only Rs. 75 will be charged; if submitted within 90 days after that, an additional 25% penalty will be charged
75	75	The person resigning must be present at the office and submit an application with signature as per Form No. 11
76	76	If only one trade union exists in an establishment, the concerned trade union may apply for official recognition 3 months after registration, If two or more trade unions are registered in an establishment, the process for official trade union election will be conducted; management and trade unions must apply for the election process
77	77	Manage and safeguard office goods and materials
78	78	Implement as per the provisions of the Child Labour Act 2056
79	79	Maintain records of deposits and appropriations and perform related tasks, Take action for settlement of arrears
80	80	Register incoming letters and dispatch outgoing letters from the office
81	81	Application and tenant certificate, Recommendation from local authority, Death registration, Photocopy of relationship certificate, Photocopy of citizenship certificate, Receipt of crop payment, Consent if other heirs, Consent of landowner, Other necessary documents
82	82	Claim application, Photocopy of landowner and tenant certificate, Land revenue receipt, Receipt of crop payment, Map, Other necessary documents
83	83	Tenant must pay crop to landowner, If not deposited or paid within legal period, landowner can file complaint with supporting documents, Landowner certificate, Tax payment receipt, Citizenship certificate
84	84	Only applications before 2054/3/23 will be processed, Consent document, Receipt of crop payment, Recommendation from local authority, Copy of citizenship certificate, Other necessary documents as required Documents
85	85	If landowner prevents tenant from cultivating, tenant must apply within 35 days, Consent or tenant right certificate, Receipt of crop payment, Recommendation from local authority if possible, Copy of citizenship certificate
86	86	Claim application, Recommendation from local authority, Copy of citizenship certificate, Public notice for claim as required Documents, Landowner certificate, Tax payment receipt, Photocopy
87	87	As per submitted Form No. 7, details of all land in the name of the person and family across the country must be disclosed, Match with Form No. 1, verify family number, match with field book and records, after all details are verified, submit to Land Ceiling Settlement Committee
88	88	Tenant must deposit the determined value of crop in Land Reform Office or municipality/VDC within the legal period each year, mentioning name and address of landowner, with tenant certificate and citizenship
89	89	Landowner must apply for return of crop deposit made by tenant, with landowner certificate, land tax receipt, and copy of citizenship
90	90	Application with proof of concerned person eligible for copy issuance, and recommendation as required Documents
91	91	Approved mortgage deed, Letter from government office, court, or financial institution for lien or release, Original landowner certificate
92	92	Original land ownership certificate, Original citizenship certificate and copies (of both buyer and seller), Current fiscal year's land tax payment receipt, Tax clearance certificate if includes house and deed document
93	93	Original land ownership certificate, Current fiscal year's land tax receipt, Relationship proof, Death certificate, Consent letter if multiple heirs, Recommendation from municipality/VDC of permanent address, Public notice and local inquiry for heirs other than spouse and children
94	94	Previous Raikar conversion records, Field book, Map, Municipality/VDC recommendation, Relationship proof and death certificate for heir claims, Municipality/VDC recommendation showing permanent residence and heirs, Land Form No. 7 if possible, Public notice for claim rights, Citizenship certificate and other necessary documents
95	95	Pota Birta records, Application within legal deadline, Tenant statement, Field book, Field map, Land Form No. 7 if possible, Municipality/VDC recommendation, For name transfer - relationship proof, death certificate, permanent residence recommendation, public notice for claims and other necessary documents
96	96	Registration application by end of 2053 Ashar, Tenant certificate, Field book, Field map, Land Form No. 2 schedule if possible, Municipality/VDC recommendation, For name transfer - relationship proof, death certificate, public notice for claims and other necessary documents
97	97	Tenant registration - Land Reform decision and notice letter, Original land ownership certificate, Citizenship certificate and other necessary documents
98	98	Registration as per court settlement - Court's final decisions and notice letter, Applicant's citizenship, complete land registration procedure if pending
99	99	Share transfer deed, recommendation, death certificate, deed validation, citizenship, land tax receipt and other necessary documents as required Documents
100	100	Name amendment (Land ownership certificate, Citizenship certificate, Specific recommendation from VDC/Municipality for name/surname difference, notice verification and other necessary documents)
101	101	Original registered deed, decision file, person must be present with 2 photos to be affixed, thumbprint required Documents
102	102	Original land ownership certificate, Citizenship certificate, Three generation details with certified photo, Recommendation from permanent residence VDC/Municipality, Source document of ownership and other necessary documents
103	103	Original land ownership certificate, Citizenship certificate, Three generation details with certified photo, Recommendation from permanent residence VDC/Municipality, Source document of ownership and other necessary documents
104	104	Copy of nursery registration certificate, Nursery inspection report, Recommendation letter from the concerned local body, Application from the nursery owner with a record of quality seedlings in the nursery
105	105	Official letter regarding annual target and demand from province/local body and farm/center, and record of quality seedlings from private nurseries
106	106	Written request by individual/institution, Telephone, Fax or in-person request
107	107	Oral or written official letter from individual/institution
108	108	Oral or written official request letter from individual/institution
109	109	Oral or written official request letter from individual/institution
110	110	Registration certificate of concerned person/agency, Copy of tax clearance certificate, Other necessary documents
111	111	Details with justification for demand
112	112	Information on approved programs and written letter for grievance
113	113	As per Civil Service Act and Rules
114	114	Oral or written official request letter from individual/institution
115	115	Application, approved order, bill payment, decision, and other necessary supporting documents
124	124	Birth certificate, Parent's citizenship certificates (original and copies), Recommendation letter from ward office, Proof of residence (e.g., utility bill or ward certification), Recent passport-size photographs
125	125	Proof of 15+ years of residence in Nepal, Marriage certificate (if through marriage), Spouse's Nepali citizenship certificate, Recommendation from ward and municipality, No objection letter from concerned embassy (if required), Recent passport-size photographs
126	126	Police report (for lost document), Original citizenship number or details, Recommendation letter from ward office, Recent passport-size photographs
127	127	Original citizenship certificate, Birth certificate or academic documents, Correction application letter, Recommendation from ward office, Affidavit or verification letter (if needed)
128	128	Proof of single parent status (death/divorce/legal separation), Single parent's citizenship certificate, Birth certificate of child, Recommendation from ward and municipality, Recent passport-size photos
129	129	Application form signed by both parties, Citizenship certificates of both individuals (copies and originals), Proof of age (birth certificate or academic record), Recommendation letter from the ward office, 2 passport-size photographs of both individuals, Marriage ceremony evidence (optional – photo or witness statement)
130	130	Application to the District Court, Citizenship certificates of both parties, Proof of marital status (bachelorhood/divorce status if applicable), Letter of consent (if under 20 years and marrying with special condition), 2 passport-size photos each, Witnesses (2–3 with ID copies), District Court approval document
131	131	All documents from normal registration, Explanation letter for the delay, Recommendation from ward and municipality, Affidavit or witness statements (if needed)
132	132	Copy of court-issued divorce decision, Citizenship certificate of applicant(s), Marriage certificate (original/copy), Recommendation letter from ward office (if required), Passport-size photographs, Application form
133	133	Application to District Court, Grounds for divorce with explanation, Marriage certificate, Citizenship certificate(s), Evidence documents (if any), Witnesses (if needed)
134	134	Application form, Proof of birth (hospital record or witness), Parent's citizenship certificates (original and copy), Marriage certificate of parents (if available), Recommendation letter from ward office, Recent photo of the child (if required)
135	135	All documents required for normal registration, Application explaining the delay, Recommendation letter from municipality, Affidavit from parents or guardians, Local inquiry or publication in some cases
136	136	Application form, Proof of death (hospital certificate or witness), Citizenship certificate of the deceased, Relationship proof with applicant, Recommendation from ward office, Post-mortem report (if applicable)
137	137	All documents required for standard registration, Letter explaining delay, Affidavit by close relative or witness, Ward/Municipality recommendation, Local publication (sometimes required)
138	138	Original birth/death certificate, Correct supporting documents (e.g., citizenship, passport, academic record), Correction application, Ward recommendation, Affidavit (if needed)
139	139	Citizenship certificate (original and copy), Ward recommendation letter (sometimes required), Blood group certificate (optional), Recent passport-size photograph (may be taken during biometrics), Biometric data (fingerprint, retina, signature taken on-site)
140	140	Police report of lost card, Citizenship certificate (original and copy), Previous national ID number (if available), Application form, Ward recommendation (if required)
141	141	Original national ID card, Corrected supporting document (e.g., citizenship, birth certificate), Application for correction, Ward recommendation (sometimes), Affidavit (if needed)
142	142	Nepali citizenship certificate, Passport and visa copy, Proof of return/migration or re-entry, Application form, Biometrics to be taken in Nepal
143	143	Registration receipt number or form ID, Citizenship number, Mobile number (optional)
\.


--
-- TOC entry 4923 (class 0 OID 16687)
-- Dependencies: 222
-- Data for Name: services; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.services (service_id, office_id, service_no, service_name, time_taken, charge, contact_section, grievance_section, monitoring_officer, responsible_officer, remarks) FROM stdin;
1	1	1	Entrance Management (Ticket Sales, Revenue Collection, Locker Facilities)	Immediate during office hours	Entrance Fee: 1) Nepali visitors: Rs. 25 per person, Students: Rs. 10 per person, Mobile/Digital camera: Rs. 50, Movie camera: Rs. 100 2) SAARC country visitors: Rs. 50 per person, Mobile/Digital camera: Rs. 75, Movie camera: Rs. 100 3) Foreign visitors: Rs. 100 per person, Mobile/Digital camera: Rs. 150, Movie camera: Rs. 200 (Free entry for museum members and students visiting on Saturday with school letter)	Administration (Ticket Counter) Room No. 1	Administration Section	Administration Section	Administration (Ticket Counter) Room No. 1	Visitor entry time: 1) Kartik 16-Magh 15: 10:30 AM to 3:30 PM 2) Magh 16-Kartik 15: 10:30 AM to 4:30 PM 3) Monday: 10:30 AM to 2:30 PM only 4) Museum closed every Tuesday and public holidays
2	1	2	Sale and distribution of replicas produced by museum and published books and other materials	Immediate during office hours	As specified	Administration (Museum Shop) Room No. 2	Administration Section	Museum Chief Room No. 8	Administration (Museum Shop) Room No. 2	-
3	1	3	Exhibition of materials in Juddha Art Gallery, information about exhibited items, conservation, security and cleaning, and providing guide service	Immediate during office hours	-	Juddha Art Gallery (Museum Officer) Room No. 3	-	Museum Chief Room No. 8	Juddha Art Gallery (Museum Officer) Room No. 3	Photography prohibited in art gallery
4	1	4	Exhibition of materials in Buddhist Art Gallery, information about exhibited items, conservation, security and cleaning, and providing guide service	Immediate during office hours	-	Buddhist Art Gallery (Museum Officer) Room No. 4	-	Museum Chief Room No. 8	Buddhist Art Gallery (Museum Officer) Room No. 4	-
5	1	5	Providing books from library for reading	Immediate during office hours	-	Library Section Room No. 5	-	Museum Chief Room No. 8	Library Section Room No. 5	-
6	1	6	Letter registration/dispatch, record keeping and staff management along with other emergency tasks	Immediate during office hours	-	Administration Section Room No. 6	-	Museum Chief Room No. 8	Administration Section Room No. 6	-
7	1	7	Financial budget preparation and financial administration related tasks	Immediate during office hours	-	Financial Administration Section Room No. 7	-	Museum Chief Room No. 8	Financial Administration Section Room No. 7	-
8	1	8	Exhibition of materials in Natural Science Gallery, information about exhibited items, conservation, security and cleaning, and providing guide service	Immediate during office hours	-	Natural Science Gallery (Museum Officer) Room No. 9	-	Museum Chief Room No. 8	Natural Science Gallery (Museum Officer) Room No. 9	-
9	1	9	Exhibition of materials in Historical Gallery, information about exhibited items, conservation, security and cleaning, and providing guide service	Immediate during office hours	-	Historical Gallery (Museum Officer) Room No. 10	-	Museum Chief Room No. 8	Historical Gallery (Museum Officer) Room No. 10	-
10	1	10	Conservation, security and systematic storage of items in museum storage	Immediate during office hours	-	Store Section (Technical) Room No. 11	-	Museum Chief Room No. 8	Store Section (Technical) Room No. 11	-
11	1	11	Record keeping and management of inventory items needed for museum	Immediate during office hours	-	Store Section Room No. 12	-	Museum Chief Room No. 8	Store Section Room No. 12	-
12	1	12	Maintaining photo records of museum items, managing photos needed for study and research	Immediate during office hours	-	Photography Section Room No. 13	-	Museum Chief Room No. 8	Photography Section Room No. 13	-
13	1	13	Chemical conservation of items in display and storage	Immediate during office hours	-	Chemical Section Room No. 14	-	Museum Chief Room No. 8	Chemical Section Room No. 14	-
14	1	14	Systematic record management of items collected through various means and distributing to different sections	Immediate during office hours	-	Chronology Section Room No. 15	-	Museum Chief Room No. 8	Chronology Section Room No. 15	-
15	1	15	Providing necessary information about the museum	Immediate during office hours	Free	Contact Officer/Information Officer	-	Museum Chief Room No. 8	Contact Officer/Information Officer	-
16	1	16	Garden management and cleaning	Immediate during office hours	-	Administration Section Room No. 6	-	Museum Chief Room No. 8	Administration Section Room No. 6	-
17	1	17	Museum security arrangements	Continuous	-	Nepal Army	-	Battalion Commander	Nepal Army	-
18	2	18	Inquiry	—	No charge	Inquiry Section, Room No. 8/302/303	Account Officer	Account Officer	Inquiry Section, Room No. 8/302/303	—
19	2	19	Providing serial number / Assistance at counter	—	No charge	Police/Office Assistant, Room No. 8/302/303	Account Officer	Account Officer	Police/Office Assistant, Room No. 8/302/303	—
20	2	20	Rapid passport service application registration and prepared passport collection, Verification, Revenue payment, Live enrollment, Prepared passport collection	2 working days	New/Renewal (34 pages): Rs. 12,000; (66 pages): Rs. 20,000\\nMinor (34 pages): Rs. 9,500; (66 pages): Rs. 14,500	Live Enrollment / Distribution Counter / Bank Staff / Computer Operator, Counter No. 2–8 / Room No. 9,10	Account Officer	Account Officer	Live Enrollment / Distribution Counter / Bank Staff / Computer Operator, Counter No. 2–8 / Room No. 9,10	—
21	2	21	Certification of Minor ID / Matching citizenship details	—	No charge	Section Officer/Computer Operator, Room No. 6	Account Officer	Account Officer	Section Officer/Computer Operator, Room No. 6	—
22	2	22	Inquiry about not receiving or understanding message/SMS	—	No charge	Computer Operator, Room No. 11A	Account Officer	Account Officer	Computer Operator, Room No. 11A	—
23	2	23	Addressing issues related to National ID Card	—	No charge	National ID Card Office Representative, Counter No. 1	Account Officer	Account Officer	National ID Card Office Representative, Counter No. 1	—
24	2	24	Registration of lost passport details	—	No charge	Computer Operator, Room No. 11B	Account Officer	Account Officer	Computer Operator, Room No. 11B	—
25	2	25	Grievance hearing and technical support	—	No charge	Section Officer / Computer Engineer / IT Officer, Room No. 210	Account Officer	Account Officer	Section Officer / Computer Engineer / IT Officer, Room No. 210	—
26	2	26	Various administrative services	—	No charge	Section Officer / Assistant Computer Operator / Office Assistant, Room No. 105	Account Officer	Account Officer	Section Officer / Assistant Computer Operator / Office Assistant, Room No. 105	—
27	2	27	Issuance or correction of birth details in old passport with visa	1 working day	Rs. — in one working day	Section Officer / Assistant Computer Operator / Office Assistant, Room No. 105/107	Account Officer	Account Officer	Section Officer / Assistant Computer Operator / Office Assistant, Room No. 105/107	—
28	2	28	Remaining passport work after application rejection in system	2 working days	No charge	Account Officer, Room No. 302/303/8	Account Officer	Account Officer	Account Officer, Room No. 302/303/8	—
29	2	29	Information on application registered in system	—	No charge	Section Officer/Computer Operator/Office Assistant, Room No. 110	Account Officer	Account Officer	Section Officer/Computer Operator/Office Assistant, Room No. 110	—
30	2	30	Information on applications under investigation	—	No charge	Section Officer (Central Investigation Unit), Room No. 111	Account Officer	Account Officer	Section Officer (Central Investigation Unit), Room No. 111	—
31	2	31	Collection of missed passport (if not collected on time)	Same day	No additional charge	Distribution Counter No. 12–14	Account Officer	Account Officer	Distribution Counter No. 12–14	—
32	2	32	Travel Permit	7 working days	Rs. 4,000	Section Officer/Assistant Computer Operator/Office Assistant, Room No. 105/107	Account Officer	Account Officer	Section Officer/Assistant Computer Operator/Office Assistant, Room No. 105/107	—
33	2	33	Issuance of diplomatic/special passport	2 working days	Rs. — in 2 working days	Assistant Computer Operator, Room No. 108	Account Officer	Account Officer	Assistant Computer Operator, Room No. 108	—
34	2	34	Forensic examination of personality and documents	—	No charge	Forensic Officer, Room No. 104	Account Officer	Account Officer	Forensic Officer, Room No. 104	—
35	2	35	Library services	—	No charge	Section Officer/Assistant Computer Operator/Office Assistant, Room No. 306/307/308	Account Officer	Account Officer	Section Officer/Assistant Computer Operator/Office Assistant, Room No. 306/307/308	—
36	2	36	Passport collection assistance services	—	No charge	Section Officer/Assistant Computer Operator/Office Assistant, Room No. 306–308	Account Officer	Account Officer	Section Officer/Assistant Computer Operator/Office Assistant, Room No. 306–308	—
37	2	37	Services related to revenue and accounts/financial administration	—	No charge	Accounts Officer/Accountant, Room No. 309	Account Officer	Account Officer	Accounts Officer/Accountant, Room No. 309	—
38	2	38	Store services	—	No charge	Assistant/Office Assistant, Room No. 312	Account Officer	Account Officer	Assistant/Office Assistant, Room No. 312	—
39	3	39	Issuing Letter Permission for Trekking	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	For lower Dolpa Area - US Dollar 10 per week per person; For Olanchungola, Lelep, Papung, Yamphudin area of Taplejung District, Kimathanka, Chepuwg, Hatiya, Pawakhola VIllage Development Committee areas in Sankhuwasabha District and all areas northwest from Thame to Nangpal of Namche Village Development Committee in Solukhumbu District US Dollar 10 per week per person for first 4 week and after 20 Dollar per week per person; US Dollar 10 per week per person in Temure and Thamun of Rashuwa District; US Dollar 90 from September to November for Nar, Fu and north side of Tilche village of Thoche village Development Committee (VDC) of Manang district and 75 US Dollar from December to August; US Dollar 90 for first seven days and after that US Dollar 15 per day per person in Mugu, Dolpu, Pulu and Vangri area of Mugu district; US Dollor 90 for first seven days and after that US Dollar 15 per day per person in Khada, Sipal and Dhuli area of Bhajang district; US Dollor 90 for first seven days and US Dollor 15 per day per person in Vyas VDC area of Darchula district; US Dollar 500 for first 10 days and after that US Dollar 50 per day per person in Upper Mustang and Upper Dolpa; Manaslu Area : (a)US Dollar 70 per week per person & then US Dollar 10 per person per day from Sept to Nov (b) US Dollar 50 per person per week and US Dollar 10 per person per week from December to August; US Dollar 50 per week per person and a fter that US Dollar 7 per day in Simikot, Yari, Limi Muchu VDC and area adjoining to the way of Tibet through Tange Khola of Dharma VDC; US Dollar 10 per person week for Gaurishankar & Lamabagar of Dolakha district; US DOllar 35 per person for first 8 days for September to November and then US Dollar 25 for December to August for Chhekampar and Chumchet area of Gorkha District.	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
40	3	40	Tourist Visa Extension	Two hours	Minimum 45 US Dollar for upto 15 days; After 15 days 3 US Dollar per days (extension of the tourist visa, the fees chargeable for 15 days shall be levied if the period is less than 15 days in every extension; Extra US Dollar 25 for MRE facility for valid visa period; Late fee to be charged Extra 5 US Dollar per day; Free of Cost incase minor below 10 years of age (Except US Passport Holders).	Administration Section (Room no. - 304)	GOVINDA P RIJAL	Department of Immigration	Administration Section (Room no. - 304)	Visa
41	3	41	Visa Transfer	Two hours	Nepalese Currency equivalent to 2 (Two) US Dollar	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Visa
42	3	42	Certifying the Arrival	Application submitted before 13 PM on same day otherwise next day before 12P	Nepalese Currency equivalent to 2 (Two) US Dollar	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Certifying Arrival/Departure
62	4	62	Inspection of pressure vessels, boilers, and other machinery	Regular	No Charge	Junior Officer	Junior Officer	Junior Officer	Junior Officer	—
43	3	43	Certifying the Departure	Application submitted before 1:00 PM on same day otherwise next day before 12:00 PM	Nepalese Currency equivalent to 2 (Two) US Dollar	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Certifying Arrival/Departure
44	3	44	Business Visa	Application submitted before 13PM same day otherwise next day before 12 PM	Investment less than NRS 10 Million - USD 30 per month; USD 300 per year, USD 1000 for five years Investment more than NRS 10 Million - USD 10 per month; USD 100 per year; USD 300 for five year per person Investment more than NRS 100 Million - Free of Cost	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Visa
45	3	45	Study Visa	Application submitted before 13PM same day otherwise next day before 12 PM	Students and Dependents None Degree USD 50 per month per person; Degree Program USD 20 per month per person; Researcher & Teacher USD 40 per month; SAARC Countries Sri Lanka & Pakistan Students GRATIS, dependents USD 10 per month Bangladesh USD 15 per year	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Visa
46	3	46	Residential Visa a. Foreigner who wants to live in Nepal without doing any business	The next day after the approval of the Home Ministry / the same day in case of renewal	Nepalese Currency Equivalent to USD 700 per year, USD 1200 for renewal charge, USD 100 for persons who have been residing in Nepal before the amendment to Rules 11 (1) of Immigration Rules in 2058 B.S. (1998 AD)	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Visa
47	3	47	Residential Visa b. The VIsa for person and his family who invest minimum of 100 thousands USD or other equivalent currency in Industrial Business of Nepal	The next day after the approval of the Home Ministry / in case of renewal in the same day Even on the basis of character / activities report submission	Nepalese currency equivalent to USD 200 per year, For renewal of same visa USD 100 per year.	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Visa
48	3	48	Non Residential Nepalese VISA (NRN VISA)	Next day of the date on which approval of the Home Ministry is obtained / the same day in case of renewal	Nepalese currency equivalent to USD 100 per year with MRE Student obtaining NRN study visa USD 25 per year with MRE	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Visa
49	3	49	Non Tourist Visa	Immediate during office hours	USD 60 per month for first year and USD 60 for next year Nepalese currency equivalent to USD 20 per month for work permit holder (Chinese) between GoN or its Organization or Business organization establish before 1989. Nepalese origin foreigner	Administration Section (Room no. - 304)	GOVINDA P RIJAL	Department of Immigration	Administration Section (Room no. - 304)	Visa
50	3	50	Lower Dolpa Area	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	USD 10 per week per person	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
51	3	51	Olanchungola, Lelep, Papung, Yamphudin area of Taplejung District, Kimathanka, Chepuwg, Hatiya, Pawakhola VIllage Development Committee areas in Sankhuwasabha District and all areas northwest from Thame to Nangpal of Namche Village Development Committee in Solukhumbu District	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	USD 10 per week per person for first 4 week and after 20 Dollar per week per person	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
52	3	52	Temure and Thamun of Rashuwa District	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	USD 10 per week per person	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
53	3	53	Nar, Fu and north side of Tilche village of Thoche village Development	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	USD 90 from September to November for first seven days and after that USD 15 per day per person	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
54	3	54	Mugu, Dolpu, Pulu and Vangri area of Mugu district	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	USD 90 for first seven days and after that USD 15 per day per person	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
55	3	55	Khada, Sipal and Dhuli area of Bhajang district	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	USD 90 for first seven days and USD 15 per day per person	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
56	3	56	Vyas VDC area of Darchula district	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	USD 90 for first seven days and after that USD 15 per day per person	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
57	3	57	Upper Mustang and Upper Dolpa	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	USD 500 for first 10 days and after that USD 50 per day per person	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
58	3	58	Manaslu Area	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	(a) USD 70 per week per person & then USD 10 per person per day from Sept to Nov (b) USD 50 per person per week and USD 10 per person per week from December to August	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
59	3	59	Simikot, Yari, Limi Muchu VDC and area adjoining to the way of Tibet through Tange Khola of Dharma VDC	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	USD 50 per week per person and a fter that USD 7 per day	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
60	3	60	Gaurishankar & Lamabagar of Dolakha district	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	USD 10 per person week for first 4 week and after that USD 20 per week per person	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
61	3	61	Chhekampar and Chumchet area of Gorkha District	Application submitted before 1:00 PM same day other wise next day before 12 PM (Nepali)	USD 35 per person for first 8 days from September to November, then USD 25 from December to August	Department of Immigration	GOVINDA P RIJAL	Department of Immigration	Department of Immigration	Issuing letter of permission for trekking
63	4	63	License for operation of pressure vessels and boilers	—	No Charge	Junior Officer	Junior Officer	Junior Officer	Junior Officer	—
64	4	64	Renewal of license for operation of pressure vessels and boilers	—	No Charge	Junior Officer	Junior Officer	Junior Officer	Junior Officer	—
65	4	65	Labor supplier's license	15 Days	Rs 10000	Junior Officer	Junior Officer	Junior Officer	Junior Officer	—
66	4	66	Renewal of labor supplier's license	1 Day	Rs 5000	Junior Officer	Junior Officer	Junior Officer	Junior Officer	—
67	4	67	Factory inspection	Regular	No Charge	Nayab Subba	Nayab Subba	Nayab Subba	Nayab Subba	—
68	4	68	Individual claims and complaints as per Section 114 (1) of the Labour Act	As per claim submission	No Charge	Nayab Subba	Nayab Subba	Nayab Subba	Nayab Subba	—
69	4	69	Collective claims and complaints as per Section 118 (1) of the Labour Act	As per claim submission	No Charge	Nayab Subba	Nayab Subba	Nayab Subba	Nayab Subba	—
70	4	70	Taking action on other petitions submitted by employers, workers, and staff	As per claim submission	No Charge	Nayab Subba	Nayab Subba	Nayab Subba	Nayab Subba	—
71	4	71	Labour administration related tasks	Regular	No Charge	Nayab Subba	Nayab Subba	Nayab Subba	Nayab Subba	—
72	4	72	Employee administration related tasks	Regular	—	Nayab Subba	Nayab Subba	Nayab Subba	Nayab Subba	—
73	4	73	Registering trade unions	—	Rs 150	Junior Officer/Kharidar	Junior Officer/Kharidar	Junior Officer/Kharidar	Junior Officer/Kharidar	—
74	4	74	Trade union registration renewal	—	Rs 75	Junior Officer/Kharidar	Junior Officer/Kharidar	Junior Officer/Kharidar	Junior Officer/Kharidar	—
75	4	75	Membership resignation	—	No Charge	Nayab Subba	Nayab Subba	Nayab Subba	Nayab Subba	—
76	4	76	Official trade union election	—	No Charge	Nayab Subba	Nayab Subba	Nayab Subba	Nayab Subba	—
77	4	77	Inventory management		No Charge	Kharidar	Kharidar	Kharidar	Kharidar	—
78	4	78	Implementation of Child Labour Act 2056		No Charge	Kharidar	Kharidar	Kharidar	Kharidar	—
79	4	79	Revenue	Regular	No Charge	Accountant	Accountant	Accountant	Accountant	—
80	4	80	Registration and dispatch	Regular	No Charge	Kharidar	Kharidar	Kharidar	Kharidar	—
81	5	81	Tenant Name Transfer	Same day if process is complete, after process if not complete	Rs. 10 ticket	Tenant Section	Tenant Section	Tenant Section	Tenant Section	-
82	5	82	Land Partition (if joint or separate claim by tenant/landowner)	After completion of process	Rs. 10 ticket	Land Partition Section	Land Partition Section	Land Partition Section	Land Partition Section	-
83	5	83	Crop Payment Tenant Eviction	After both parties are notified with 15 days' notice and evidence is submitted, decision will be made after process is complete	Rs. 50 ticket (complaint and response fee also Rs. 50 each)	Tenant Section	Tenant Section	Tenant Section	Tenant Section	-
84	5	84	Old Applications for Tenant Establishment	Rs. 50 ticket (complaint and response fee also Rs. 50 each)	After both parties are notified with 15 days' notice and evidence is submitted, decision will be made after process is complete	Tenant Section	Tenant Section	Tenant Section	Tenant Section	-
85	5	85	Tenant Eviction	Same day if process is complete, after process if not complete	As per law	Tenant Section	Tenant Section	Tenant Section	Tenant Section	-
86	5	86	Tenant Record Removal	15 days' notice issued for response	Rs. 10 ticket	Tenant Section	Tenant Section	Tenant Section	Tenant Section	-
87	5	87	Land Ceiling	After completion of process	Rs. 10 ticket	Ceiling Section	Ceiling Section	Ceiling Section	Ceiling Section	-
88	5	88	Crop Deposit	Same day	Rs. 10 ticket	Deposit Section	Deposit Section	Deposit Section	Deposit Section	-
89	5	89	Return of Crop Deposit to Landowner	Same day after verification	Rs. 10 ticket	Deposit Section	Deposit Section	Deposit Section	Deposit Section	-
90	5	90	Copy Issuance	Generally same day	As per Financial Act	Related Moth Section, Tameli Section	Related Moth Section, Tameli Section	Related Moth Section, Tameli Section	Related Moth Section, Tameli Section	-
91	5	91	Lien (Rokka)	1 to 2 hours	As per Financial Act	Lien Section	Lien Section	Lien Section	Lien Section	-
92	6	92	Registration Pass	Same day	As specified by Financial Act of each fiscal year	Registration Section Moth	Registration Section Moth	Registration Section Moth	Registration Section Moth	-
93	6	93	Name Transfer	After completion of process	Free within 35 days of death, Rs. 10 thereafter	Name Transfer Section	Name Transfer Section	Name Transfer Section	Name Transfer Section	-
94	6	94	Land Registration (Raikar Land)	After completion of process	Rs. 10 for name transfer registration and land tax receipts from previous to current fiscal year	Land Registration Section	Land Registration Section	Land Registration Section	Land Registration Section	On-site verification as needed and field inspection report by officer level for possession verification
95	6	95	Birta Land Registration for Birta holders	After completion of process	Fine from Rs. 100 to 1,000 as determined, plus Rs. 10 if name transfer after 35 days, and land tax from 2016 BS to current fiscal year	Land Registration Section	Land Registration Section	Land Registration Section	Land Registration Section	On-site verification as needed and field inspection report by officer level for possession verification
96	6	96	Birta Land Registration for tenants	After completion of process	10% of valuation amount for registration purpose, double service charge of current fiscal year's land tax	Land Registration Section	Land Registration Section	Land Registration Section	Land Registration Section	On-site verification as needed and field inspection report by officer
97	6	97	Registration	Same day if possible	As specified by law	Registration and Decision Implementation Section	Registration and Decision Implementation Section	Registration and Decision Implementation Section	Registration and Decision Implementation Section	-
98	6	98	Registration as per court decision	After completion of process	Settlement fee (as specified by Financial Act)	Registration and Decision Implementation Section	Registration and Decision Implementation Section	Registration and Decision Implementation Section	Registration and Decision Implementation Section	-
99	6	99	Share transfer registration	After completion of process	Rs. 10 after 35 days of donor's death, free before that	Registration and Decision Implementation Section	Registration and Decision Implementation Section	Registration and Decision Implementation Section	Registration and Decision Implementation Section	On-site verification for share transfer deeds without land details
100	6	100	Amendment	After completion of process	As specified by Financial Act	Amendment Section	Amendment Section	Amendment Section	Amendment Section	-
101	6	101	Record Registration and Cancellation	Generally same day	Free	Moth Section	Moth Section	Moth Section	Moth Section	-
102	6	102	Photo Affixation	Generally same day	Free	Moth Section	Moth Section	Moth Section	Moth Section	-
103	6	103	Three Generation Details	Generally same day	Free	Moth Section	Moth Section	Moth Section	Moth Section	-
104	7	104	Determination of Fruit Seedling Source Center	For winter fruits, by Kartik 15; for summer fruits, by Chaitra end, to be submitted to the center with recommendation from the local level.	No charge	Section Chief, Room No. 5	Chief	Chief	Section Chief	Technical Branch
105	7	105	Annual collection of fruit seedling demand and management for supply	For winter fruits, by the end of Kartik; for summer fruits, by Baisakh 15, the center must prepare the balance sheet and complete correspondence.	No charge	Section Chief, Room No. 5	Chief	Chief	Section Chief	Technical Branch
106	7	106	Technical service delivery for fruit nursery and special cultivation	As per demand received	No charge	Section Chief, Room No. 5	Chief	Chief	Section Chief	Technical Branch
107	7	107	Oral or written official letter from individual/institution	Updated annually and provided as per demand	No charge	Section Chief, Room No. 5	Chief	Chief	Section Chief	Technical Branch
108	7	108	Management and distribution of fruit-related technology publications	Prepared annually/quarterly and provided immediately as per demand	No charge	Section Chief, Room No. 5	Chief	Chief	Section Chief	Technical Branch
109	7	109	Provision of technical information and problem solving related to fruits	Service provided as soon as demand is received	No charge	Horticulture Development Officer (Bishnu Prasad Giri), Room No. 12	Chief	Chief	Horticulture Development Officer (Bishnu Prasad Giri)	Technical Branch
110	7	110	Coordination and partnership/collaboration with various agencies for fruit development and extension	Regularly as needed	No charge	Section Chief, Room No. 8	Chief	Chief	Section Chief	Technical Branch
111	7	111	Processing and facilitation of budget and program-related applications	Service provided as soon as demand is received	No charge	Section Chief, Room No. 8	Chief	Chief	Section Chief	Technical Branch
112	7	112	Monitoring/evaluation and grievance management	Within 3 days of submission of grievance, as per need and rules, as soon as possible	No charge	Section Chief, Room No. 8	Chief	Chief	Section Chief	Technical Branch
113	7	113	Administrative information and service delivery	Service provided as soon as demand is received	No charge	Section Chief, Room No. 7	Chief	Chief	Section Chief	Technical Branch
114	7	114	Distribution of annual progress reports and booklets published by the center	Service provided as soon as demand is received	No charge	Section Chief, Room No. 7	Chief	Chief	Section Chief	Technical Branch
115	7	115	Management of center-related payment disbursement	Within 3 days of submission of required documents	No charge	Section Chief, Room No. 7	Chief	Chief	Section Chief	Technical Branch
116	8	116	Determination/Update of Public Vehicle Fare	As per the specific case	Free	Director/CDE/ME Technical Branch and Administration, Planning, Monitoring and Evaluation Branch	Director/CDE/ME Technical Branch and Administration, Planning, Monitoring and Evaluation Branch	Director/CDE/ME Technical Branch and Administration, Planning, Monitoring and Evaluation Branch	Great, Chief, or Principal/Director	-
117	8	117	Issue Road Worthiness Certificate	Not more than 15 days	Free	Director/ME Technical Branch	Director/ME Technical Branch	Director/ME Technical Branch	Great, Chief, or Principal/Director	-
118	8	118	Transport Business Association, Organization Registration Related Opinion/Feedback	Not more than 15 days	Free	Director/Section Officer Vehicle Monitoring Standards Formulation and Driver Management, Inter-Provincial Coordination Study/Research Branch	Director/Section Officer Vehicle Monitoring Standards Formulation and Driver Management, Inter-Provincial Coordination Study/Research Branch	Director/Section Officer Vehicle Monitoring Standards Formulation and Driver Management, Inter-Provincial Coordination Study/Research Branch	Great, Chief, or Principal/Director	-
119	8	119	Factory or Workshop Registration and Renewal	Not more than 15 days	Rs. 1000 to Rs. 4000	Director/Section Officer Vehicle Monitoring Standards Formulation and Driver Management, Inter-Provincial Coordination Study/Research Branch / Director/ME Technical Branch	Director/Section Officer Vehicle Monitoring Standards Formulation and Driver Management, Inter-Provincial Coordination Study/Research Branch / Director/ME Technical Branch	Director/Section Officer Vehicle Monitoring Standards Formulation and Driver Management, Inter-Provincial Coordination Study/Research Branch / Director/ME Technical Branch	Great, Chief, or Principal/Director	-
120	8	120	Regarding Instructions Requested from Transport Management Office	Not more than 3 days	Free	Director/Section Officer Administration, Planning, Monitoring and Evaluation Branch	Director/Section Officer Administration, Planning, Monitoring and Evaluation Branch	Director/Section Officer Administration, Planning, Monitoring and Evaluation Branch	Great, Chief, or Principal/Director	-
121	8	121	Provide Vehicle Related Statistics	Same day	Free	Director/Section Officer Administration, Planning, Monitoring and Evaluation Branch	Director/Section Officer Administration, Planning, Monitoring and Evaluation Branch	Director/Section Officer Administration, Planning, Monitoring and Evaluation Branch	Great, Chief, or Principal/Director	-
122	8	122	Provide Approved Fare Rates for Passenger Public Vehicles	Same day	Free	Director/Section Officer Vehicle Monitoring Standards Formulation and Driver Management, Inter-Provincial Coordination Study/Research Branch	Director/Section Officer Vehicle Monitoring Standards Formulation and Driver Management, Inter-Provincial Coordination Study/Research Branch	Director/Section Officer Vehicle Monitoring Standards Formulation and Driver Management, Inter-Provincial Coordination Study/Research Branch	Great, Chief, or Principal/Director	-
123	8	123	Verify and Approve New Vehicles According to Nepal Vehicle Pollution Standards, 2069	Maximum 1 month	Free	Director/ME Technical Branch	Director/ME Technical Branch	Director/ME Technical Branch	Great, Chief, or Principal/Director	-
124	9	124	Citizenship Certificate (By Descent)	Same day or within a few working days	Free (may require small admin charges for recommendation)	Citizenship Section, DAO	Citizenship Section, DAO	Citizenship Officer	Citizenship Officer	Both parents must be Nepali citizens or provide legal documentation
125	9	125	Citizenship Certificate (Naturalized)	Within a few weeks after verification	As per Financial Act	Citizenship Section, DAO	Citizenship Section, DAO	Citizenship Officer	Citizenship Officer	Subject to Ministry-level verification and approval
126	9	126	Citizenship Certificate (Replacement / Lost)	Same day to 2–3 working days	Rs. 100–200 (as per district office)	Records Section, DAO	Records Section, DAO	Records Officer	Records Officer	Applicant must be physically present and biometrically verified
127	9	127	Citizenship Certificate Correction (Name, DOB, etc.)	1–5 working days depending on the case	Nominal fee (Rs. 100–300)	Citizenship Correction Section, DAO	Citizenship Correction Section, DAO	Correction Officer	Correction Officer	May require publication in local newspaper for major corrections
128	9	128	Citizenship for Children of Single Parent	2–5 working days	Free or minimal fee	Citizenship Section, DAO	Citizenship Section, DAO	Citizenship Officer	Citizenship Officer	Varies slightly based on province and district interpretation
129	10	129	Marriage Registration (General)	Same day or within 1–3 working days	Rs. 100–500 (varies by municipality)	Vital Registration Section, Ward Office	Vital Registration Section, Ward Office	Registration Officer	Registration Officer	Must be registered within 35 days of marriage as per law
130	11	130	Court Marriage (Inter-caste / Inter-national / Late Registration)	5–10 working days (includes court date)	Rs. 500–1,000 (including legal service fees)	Marriage Registration Unit, District Court	Marriage Registration Unit, District Court	Court Registrar	Court Registrar	Court-verified marriage certificate is legally binding
131	10	131	Late Marriage Registration (After 35 Days)	3–7 working days after verification	Rs. 500–1,000 depending on delay	Vital Records Section, Municipality	Vital Records Section, Municipality	Records Officer	Records Officer	Delays must be justified with valid reasons
132	10	132	Divorce Registration (Court Approved)	Same day to 2 working days after submission	Rs. 100–300 (varies by ward)	Vital Records Section, Ward Office	Vital Records Section, Ward Office	Registration Officer	Registration Officer	Divorce must first be finalized through District Court
133	11	133	Divorce Case Filing (Legal Process)	Varies – can take 1 month to several months	Rs. 1,000+ (depends on court and legal help)	Family Court Division, District Court	Family Court Division, District Court	Court Registrar	Court Registrar	Final decision is made by a judge
134	10	134	Birth Certificate Registration	Same day or within 1–2 working days	Free if registered within 35 days of birth; Rs. 100–500 if delayed	Vital Registration Section, Ward Office	Vital Registration Section, Ward Office	Registration Officer	Registration Officer	Registration is mandatory within 35 days of birth
135	10	135	Late Birth Registration (After 35 days)	3–7 working days after verification	Rs. 100–500 depending on delay period	Ward Office + Municipality Registration Section	Ward Office + Municipality Registration Section	Verification Officer	Verification Officer	Verification by local officials may be required
136	10	136	Death Certificate Registration	Same day or within 1–2 working days	Free within 35 days of death; Rs. 100+ if delayed	Vital Registration Section, Ward Office	Vital Registration Section, Ward Office	Registration Officer	Registration Officer	Mandatory for legal procedures (inheritance, insurance, etc.)
137	10	137	Late Death Registration (After 35 days)	3–7 working days	Rs. 100–500 depending on delay period	Vital Records Section, Municipality Office	Vital Records Section, Municipality Office	Records Officer	Records Officer	Legal delay must be justified and verified
138	10	138	Correction in Birth or Death Record	1–5 working days	Rs. 100–200	Vital Records Correction Unit, Municipality	Vital Records Correction Unit, Municipality	Correction Officer	Correction Officer	Major changes may need approval from District Office
139	9	139	National ID Card Registration (First-Time)	10–30 days depending on district	Free for first-time registration	National ID Registration Section, DAO	National ID Registration Section, DAO	ID Officer	ID Officer	Must be 16+ years old and possess a citizenship certificate
140	9	140	National ID Card Replacement (Lost/Damaged)	7–20 working days	Rs. 200–500 depending on district and urgency	ID Card Replacement Unit, DAO	ID Card Replacement Unit, DAO	Replacement Officer	Replacement Officer	New card issued after identity verification
141	9	141	National ID Card Correction (Name, DOB, etc.)	7–15 working days	Rs. 100–300	Correction Section, National ID Unit, DAO	Correction Section, National ID Unit, DAO	Correction Officer	Correction Officer	Major corrections may require approval from central authority
142	9	142	National ID Card for Abroad Nepali (NRN or returnees)	15–30 days	Rs. 500+ (varies)	Foreign Services Unit, National ID Dept., DAO	Foreign Services Unit, National ID Dept., DAO	Foreign Services Officer	Foreign Services Officer	Must be physically present in Nepal for biometrics
143	12	143	Tracking / Status Inquiry of National ID Card	Real-time or within 1 working day	Free	Help Desk, DAO or National ID Portal	Help Desk, DAO or National ID Portal	Help Desk Officer	Help Desk Officer	Also trackable at login.nidmc.gov.np
\.


--
-- TOC entry 4935 (class 0 OID 0)
-- Dependencies: 217
-- Name: ministries_ministry_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.ministries_ministry_id_seq', 1, false);


--
-- TOC entry 4936 (class 0 OID 0)
-- Dependencies: 219
-- Name: offices_office_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.offices_office_id_seq', 4, true);


--
-- TOC entry 4937 (class 0 OID 0)
-- Dependencies: 223
-- Name: required_documents_document_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.required_documents_document_id_seq', 143, true);


--
-- TOC entry 4938 (class 0 OID 0)
-- Dependencies: 221
-- Name: services_service_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.services_service_id_seq', 80, true);


--
-- TOC entry 4762 (class 2606 OID 16671)
-- Name: ministries ministries_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ministries
    ADD CONSTRAINT ministries_pkey PRIMARY KEY (ministry_id);


--
-- TOC entry 4764 (class 2606 OID 16680)
-- Name: offices offices_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.offices
    ADD CONSTRAINT offices_pkey PRIMARY KEY (office_id);


--
-- TOC entry 4769 (class 2606 OID 16708)
-- Name: required_documents required_documents_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.required_documents
    ADD CONSTRAINT required_documents_pkey PRIMARY KEY (document_id);


--
-- TOC entry 4766 (class 2606 OID 16694)
-- Name: services services_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.services
    ADD CONSTRAINT services_pkey PRIMARY KEY (service_id);


--
-- TOC entry 4767 (class 1259 OID 16714)
-- Name: idx_required_docs_service_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_required_docs_service_id ON public.required_documents USING btree (service_id);


--
-- TOC entry 4770 (class 2606 OID 16681)
-- Name: offices offices_ministry_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.offices
    ADD CONSTRAINT offices_ministry_id_fkey FOREIGN KEY (ministry_id) REFERENCES public.ministries(ministry_id) ON DELETE CASCADE;


--
-- TOC entry 4772 (class 2606 OID 16709)
-- Name: required_documents required_documents_service_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.required_documents
    ADD CONSTRAINT required_documents_service_id_fkey FOREIGN KEY (service_id) REFERENCES public.services(service_id) ON DELETE CASCADE;


--
-- TOC entry 4771 (class 2606 OID 16695)
-- Name: services services_office_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.services
    ADD CONSTRAINT services_office_id_fkey FOREIGN KEY (office_id) REFERENCES public.offices(office_id) ON DELETE CASCADE;


-- Completed on 2025-07-11 19:09:04

--
-- PostgreSQL database dump complete
--

