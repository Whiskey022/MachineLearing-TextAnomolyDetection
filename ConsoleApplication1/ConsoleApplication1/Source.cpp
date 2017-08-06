#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cctype>


using namespace std;

struct TermResults {
	string term;
	vector<double> TF;
	double IDF;
	vector<double> TFIDF;
	vector<double> gaussianFuncResults;
	double finalResult;
	double F1;
};

vector<string> readWordsFromFile(string fileName);
vector<string> removeNonAlphabeticLetters(vector<string> words);
vector<vector<string>> getAllDocWords(vector<string> fileNames);
vector<string> getTermsFromDoc(vector<string> vec);
vector<string> allTermsTo1DVector(vector<vector<string>> terms);
double getTF(vector<string> words, string term);
double getIDF(vector<vector<string>> docWords, string term);
vector<double> getMeanTFIDF(vector<TermResults> termResults);
vector<double> getVarianceTFIDF(vector<TermResults> termResults, vector<double> mean);
vector<vector<double>> gaussianFunction(vector<TermResults> termResults, vector<double> mean, vector<double> variance);
int main() {

	vector<string> fileNames = { "D1.txt", "D2.txt", "D3.txt", "D4.txt" , "D5.txt" ,"D6.txt" , "D7.txt" , "D8.txt" , "D9.txt" , "D10.txt" };
	vector<vector<string>> docWords = getAllDocWords(fileNames);
	vector<vector<string>> allDocsTerms;
	for (int i = 0; i < fileNames.size(); i++) {
		allDocsTerms.push_back(getTermsFromDoc(docWords[i]));
	}

	vector<string> allTerms = allTermsTo1DVector(allDocsTerms);

	vector<TermResults> termResults;

		
	for (int i = 0; i < allTerms.size(); i++) {
		TermResults temp;
		temp.term = allTerms[i];
		for (int k = 0; k < fileNames.size(); k++) {
			temp.TF.push_back(getTF(docWords[k], allTerms[i]));
		}
		temp.IDF = getIDF(docWords, allTerms[i]);
		for (int k = 0; k<temp.TF.size(); k++) {
			temp.TFIDF.push_back(temp.IDF * temp.TF[k] * 10000);
		}

		termResults.push_back(temp);	
		cout << i << " " << allTerms.size() << endl;
	}

	vector<double> meanTFIDF = getMeanTFIDF(termResults);
	vector<double> varianceTFIDF = getVarianceTFIDF(termResults, meanTFIDF);
	for (int i = 0; i < meanTFIDF.size(); i++) {
		cout << meanTFIDF[i] << " " << varianceTFIDF[i]<< endl;
	}

	for (int i = 0; i < termResults.size(); i++) {
		cout << termResults[i].term << " " << allTerms[i] << " ";
		for (int k = 0; k < termResults[i].TF.size(); k++) {
			cout << termResults[i].TF[k] << " ";
		}
		cout << "IDF: " << termResults[i].IDF << " ";
		for (int k = 0; k < termResults[i].TF.size(); k++) {
			cout << "TFIDF: " << termResults[i].TFIDF[k] << " ";
		}
		cout << endl;		
	}
	
	vector<vector<double>> gaussianFuncResults = gaussianFunction(termResults, meanTFIDF, varianceTFIDF);

	for (int i = 0; i < gaussianFuncResults.size(); i++) {
		termResults[i].gaussianFuncResults = gaussianFuncResults[i];

	}

	//Get final score
	for (int i = 0; i < termResults.size(); i++) {
		double sum = 0;
		for (int j = 0; j < termResults[i].gaussianFuncResults.size(); j++) {
			cout << termResults[i].gaussianFuncResults[j] << "-";
			sum += termResults[i].gaussianFuncResults[j];
		}
		cout << endl;
		termResults[i].finalResult = sum;
	}

	
	for (int i = 0; i < termResults.size(); i++) {
		for (int j = i+1; j < termResults.size(); j++) {
			if (termResults[i].finalResult < termResults[j].finalResult) {
				TermResults temp = termResults[i];
				termResults[i] = termResults[j];
				termResults[j] = temp;
			}
		}
	}

	ofstream write("data.txt");

	for (int i = 0; i < termResults.size(); i++) {
		write << termResults[i].term << " " << termResults[i].finalResult << endl;
	}


	return 0;
}


vector<string> readWordsFromFile(string fileName)
{
	ifstream read(fileName);

	string t;
	vector<string> words;

	while (!read.eof()) {
		getline(read, t);
		istringstream iss(t);
		string word;
		while (iss >> word) {
			words.push_back(word);
		}
	}

	return words;
}

vector<string> removeNonAlphabeticLetters(vector<string> words)
{
	for (int j = 0; j < words.size(); j++) {
		string temp;
		for (char c : words[j]) {
			if (std::isalnum(c)) { 
				if (!isupper(c)) {
					temp += c;
				}
				else {
					c = tolower(c);
					temp += c;
				}
			}
		}
		words[j] = temp;	
	}

	return words;
}

vector<vector<string>> getAllDocWords(vector<string> fileNames)
{
	vector<vector<string>> docWords;
	for (int i = 0; i < fileNames.size(); i++) {
		docWords.push_back(readWordsFromFile(fileNames[i]));
		docWords[i] = removeNonAlphabeticLetters(docWords[i]);
		sort(docWords[i].begin(), docWords[i].end());
	}

	return docWords;
};

vector<string> getTermsFromDoc(vector<string> vec)
{
	sort(vec.begin(), vec.end());
	vec.erase(unique(vec.begin(), vec.end()), vec.end());

	return vec;
}

vector<string> allTermsTo1DVector(vector<vector<string>> terms)
{
	vector<string> allTerms;
	for (int i = 0; i < terms.size(); i++) {
		vector<string> termsTemp = terms[i];
		for (int j = 0; j < termsTemp.size(); j++) {
			allTerms.push_back(termsTemp[j]);
		}
	}

	sort(allTerms.begin(), allTerms.end());
	allTerms.erase(unique(allTerms.begin(), allTerms.end()), allTerms.end());

	return allTerms;
}

double getTF(vector<string> words, string term)
{
	int termCount = 0;
	for (int i = 0; i < words.size(); i++) {
		if (words[i] == term) {
			termCount++;
		}
		else if (words[i] > term) {
			break;
		}
	}
	return termCount /(double) words.size();
}

double getIDF(vector<vector<string>> docWords, string term)
{
	double IDF;
	int docsWithTerm = 0;
	for (int i = 0; i < docWords.size(); i++) {
		vector<string> words = docWords[i];
		for (int j = 0; j<words.size(); j++) {			
			if (words[j] == term) {
				docsWithTerm++;
				break;
			}
			else if (words[j] > term) {
				break;
			}
		}
	}
	IDF = log10(docWords.size() / (double)docsWithTerm);
	cout << term<< " " << IDF << " " << (double)docsWithTerm << " " << docWords.size() / (double)docsWithTerm << endl;

	return IDF;
}

vector<double> getMeanTFIDF(vector<TermResults> termResults)
{
	vector<double> mean;

	for (int i = 0; i < termResults[0].TFIDF.size(); i++) {
		double sum = 0;
		int count = 0;
		for (int j = 0; j < termResults.size(); j++) {
			if (termResults[j].TFIDF[i] != 0) {
				sum += termResults[j].TFIDF[i];
				count++;
			}
		}
		mean.push_back(sum / count);
	}

	return mean;
}

vector<double> getVarianceTFIDF(vector<TermResults> termResults, vector<double> mean) {
	vector<double> variance;

	for (int i = 0; i < termResults[0].TFIDF.size(); i++) {
		double sum = 0;
		int count = 0;
		for (int j = 0; j < termResults.size(); j++) {
			if (termResults[j].TFIDF[i] != 0) {
				sum += pow(termResults[j].TFIDF[i] - mean[i], 2);
				count++;
			}
		}
		variance.push_back(sum / count);
	}

	return variance;
}

vector<vector<double>> gaussianFunction(vector<TermResults> termResults, vector<double> mean, vector<double> variance) {
	double result;
	vector<vector<double>> allResults;
	double pi = atan(1) * 4;
	for (int i = 0; i < termResults.size(); i++) {
		vector<double> results;
		double expPower = 0;
		for (int j = 0; j < termResults[i].TFIDF.size(); j++) {
			if (termResults[i].TFIDF[j] < mean[j]) {
				termResults[i].TFIDF[j] = mean[j];
			}
			expPower = -(pow(termResults[i].TFIDF[j] - mean[j], 2) / (2 * pow(variance[j], 2)));
			//cout << expPower << endl;
			//cout << (1 / (sqrt(2 * pi * pow(variance[j], 2))))<<endl;
			//cout << variance[j] << " " << mean[j] << " " << termResults[i].TFIDF[j] << endl;
			result = (1 / (sqrt(2 * pi * pow(variance[j], 2)))) * pow(exp(1.0), expPower);
			cout << result << endl;
			results.push_back(result);
		}
		cout << termResults[i].term << endl;
		allResults.push_back(results);
	}
	return allResults;
}