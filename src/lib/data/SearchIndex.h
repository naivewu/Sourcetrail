#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include <deque>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <vector>

#include "utility/text/Dictionary.h"
#include "utility/types.h"

class SearchIndex
{
public:
	class SearchNode;

	struct SearchResult
	{
		SearchResult();
		SearchResult(size_t weight, const SearchIndex::SearchNode* node, const SearchIndex::SearchNode* parent);

		bool operator()(const SearchResult& lhs, const SearchResult& rhs) const;

		size_t weight;
		const SearchIndex::SearchNode* node;
		const SearchIndex::SearchNode* parent;
	};

	typedef std::set<SearchResult, SearchResult> SearchResults;
	typedef SearchResults::const_iterator SearchResultsIterator;

	struct SearchMatch
	{
		void print(std::ostream& ostream) const;

		std::string encodeForQuery() const;

		std::string fullName;
		std::set<Id> tokenIds;
		std::vector<size_t> indices;
		size_t weight;
	};

	class SearchNode
	{
	public:
		typedef std::multimap<size_t, const SearchIndex::SearchNode*> FuzzyMap;
		typedef FuzzyMap::const_iterator FuzzyMapIterator;

		SearchNode(SearchNode* parent, const std::string& name, Id nameId);
		~SearchNode();

		const std::string& getName() const;
		std::string getFullName() const;

		Id getNameId() const;

		Id getFirstTokenId() const;
		const std::set<Id>& getTokenIds() const;
		void addTokenId(Id tokenId);

		SearchNode* getParent() const;
		std::deque<SearchIndex::SearchNode*> getParentsWithoutTokenId();

		const std::set<std::shared_ptr<SearchNode>>& getChildren() const;

		SearchResults runFuzzySearch(const std::string& query, bool recursive) const;

	private:
		// Accessed by SearchIndex
		std::shared_ptr<SearchNode> addNodeRecursive(std::deque<Id>* nameIds, const Dictionary& dictionary);
		std::shared_ptr<SearchNode> getNodeRecursive(std::deque<Id>* nameIds) const;

		friend class SearchIndex;

		FuzzyMap fuzzyMatchRecursive(const std::string& query, size_t pos, size_t weight, size_t size) const;
		std::pair<size_t, size_t> fuzzyMatch(
			const std::string query, size_t start, size_t size, std::vector<size_t>* indices = nullptr) const;
		SearchMatch fuzzyMatchData(const std::string& query, const SearchNode* parent) const;

		std::shared_ptr<SearchIndex::SearchNode> getChildWithNameId(Id nameId) const;
		std::deque<const SearchNode*> getNodesToParent(const SearchNode* parent) const;

		std::set<std::shared_ptr<SearchNode>> m_nodes;
		SearchNode* m_parent;

		std::set<Id> m_tokenIds;

		const std::string& m_name;
		const Id m_nameId;
	};

	static std::vector<SearchMatch> getMatches(const SearchResults& searchResults, const std::string& query);

	static void logMatches(const std::vector<SearchMatch>& matches, const std::string& query);

	SearchIndex();
	virtual ~SearchIndex();

	void clear();

	Id getWordId(const std::string& word);
	const std::string& getWord(Id wordId) const;

	SearchNode* addNode(std::vector<std::string> nameHierarchy);
	SearchNode* getNode(const std::string& fullName) const;

	SearchResults runFuzzySearch(const std::string& query) const;
	std::vector<SearchMatch> runFuzzySearchAndGetMatches(const std::string& query) const;

	static const std::string DELIMITER;

private:
	SearchNode m_root;
	Dictionary m_dictionary;
};

#endif // SEARCH_INDEX_H
