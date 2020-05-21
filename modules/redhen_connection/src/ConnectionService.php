<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Access\AccessResultAllowed;
use Drupal\Core\Access\AccessResultNeutral;
use Drupal\Core\Database\Connection as DBConnection;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Entity\Query\QueryFactory;
use Drupal\Core\Entity\Query\QueryInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\redhen_connection\Entity\ConnectionType;
use Drupal\redhen_connection\Entity\Connection;
use Drupal\redhen_contact\Entity\Contact;

/**
 * Provides an interface for getting connections between entities.
 */
class ConnectionService implements ConnectionServiceInterface {


  /**
   * The entity manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * The entity query.
   *
   * @var \Drupal\Core\Entity\Query\QueryFactory
   */
  protected $entityQuery;

  /**
   * The database connection to use.
   *
   * @var \Drupal\Core\Database\Connection
   */
  protected $connection;

  /**
   * Constructs a EntityCreateAccessCheck object.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity manager.
   * @param \Drupal\Core\Entity\Query\QueryFactory $entity_query
   *   The entity query.
   * @param \Drupal\Core\Database\Connection $connection
   *   The database connection.
   */
  public function __construct(EntityTypeManagerInterface $entity_type_manager, QueryFactory $entity_query, DBConnection $connection) {
    $this->entityTypeManager = $entity_type_manager;
    $this->entityQuery = $entity_query;
    $this->connection = $connection;
  }

  /**
   * {@inheritdoc}
   */
  public function getConnectionEntityTypes(array $entity_types) {
    $all_connection_types = [];
    foreach ($entity_types as $entity_type => $type) {
      $query = $this->entityQuery->get('redhen_connection_type');
      $or_group = $query->orConditionGroup();

      $or_group->condition('endpoints.1.entity_type', $entity_type);
      $or_group->condition('endpoints.2.entity_type', $entity_type);

      $query->condition($or_group);
      $results = $query->execute();

      if (!empty($results)) {
        $connection_types = ConnectionType::loadMultiple($results);
        $all_connection_types = array_merge($all_connection_types, $connection_types);
      }
    }

    $connected_entities = [];
    foreach ($all_connection_types as $connection_type_id => $connection_type) {
      $endpoint_1 = $connection_type->get('endpoints')[1]['entity_type'];
      $endpoint_2 = $connection_type->get('endpoints')[2]['entity_type'];
      $connected_entities[$connection_type_id]['endpoint_1'][$endpoint_1] = $entity_types[$endpoint_1];
      $connected_entities[$connection_type_id]['endpoint_2'][$endpoint_2] = $entity_types[$endpoint_2];
    }

    return $connected_entities;
  }

  /**
   * {@inheritdoc}
   */
  public function getConnectionTypes(EntityInterface $entity, EntityInterface $entity2 = NULL) {
    $query = $this->entityQuery->get('redhen_connection_type');
    $or_group = $query->orConditionGroup();
    $entity_type = $entity->getEntityTypeId();

    if (empty($entity2)) {
      // Single entity provided
      $or_group->condition('endpoints.1.entity_type', $entity_type);
      $or_group->condition('endpoints.2.entity_type', $entity_type);
    }
    else {
      // Two entities provided.
      $entity_type2 = $entity2->getEntityTypeId();
      $and_group = $query->andConditionGroup()
        ->condition('endpoints.1.entity_type', $entity_type)
        ->condition('endpoints.2.entity_type', $entity_type2);

      $and_group2 = $query->andConditionGroup()
        ->condition('endpoints.2.entity_type', $entity_type)
        ->condition('endpoints.1.entity_type', $entity_type2);

      $or_group->condition($and_group)
        ->condition($and_group2);
    }

    $query->condition($or_group);
    $results = $query->execute();

    $connection_types = [];
    if (!empty($results)) {
      $connection_types = ConnectionType::loadMultiple($results);
    }

    return $connection_types;
  }

  /**
   * {@inheritdoc}
   */
  public function getConnections(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL, $active = TRUE, $sort = [], $offset = 0, $limit = 0) {
    $connections = [];

    $query = $this->buildQuery($entity, $entity2, $connection_type, $active);

    if ($query) {

      foreach ($sort as $field => $direction) {
        $query->sort($field, $direction);
      }

      if ($limit > 0) {
        $query->range($offset, $limit);
      }

      $results = $query->execute();

      if (!empty($results)) {
        $connections = Connection::loadMultiple($results);
      }
    }

    return $connections;
  }

  /**
   * {@inheritdoc}
   */
  public function getConnectionCount(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL) {
    $query = $this->buildQuery($entity, $entity2, $connection_type);

    return $query->count()->execute();
  }

  /**
   * {@inheritdoc}
   */
  public function getConnectedEntities(EntityInterface $entity, $connection_type = NULL) {
    $connected_entities = [];

    $type = ConnectionType::load($connection_type);

    // Get all fields that reference endpoints on the connection type.
    $fields = $type->getAllEndpointFields();

    // Get connections.
    $connections = $this->getConnections($entity, NULL, $connection_type);

    // Loop through connections to find entities referenced by endpoint fields.
    foreach ($connections as $connection) {
      foreach ($fields as $field) {
        $referenced_entities = $connection->get($field)->referencedEntities();

        foreach ($referenced_entities as $referenced_entity) {
          // Do not include the entity originally passed in function args.
          if (($referenced_entity->getEntityType()->id() == $entity->getEntityType()->id()) && ($referenced_entity->id() == $entity->id())) {
            continue;
          }

          $connected_entities[] = $referenced_entity;
        }
      }
    }

    return $connected_entities;
  }

  /**
   * {@inheritdoc}
   */
  public function getIndirectConnections(EntityInterface $entity, EntityInterface $entity2, $connection_type = NULL, $active = TRUE) {
    $types = ($connection_type) ? [$connection_type => ConnectionType::load($connection_type)] : $this->getConnectionTypes($entity);

    $results = [];
    foreach ($types as $type_id => $connection_type) {
      /** @var ConnectionType $connection_type */
      // Get endpoints (usually 1, but two possible).
      $endpoints = $connection_type->getEndpointFields($entity->getEntityTypeId());
      // Use first endpoint only.
      $endpoint = current($endpoints);

      // @todo Don't hardcode the endpoint names.
      $shared_endpoint = ($endpoint == 'endpoint_1') ? 'endpoint_2' : 'endpoint_1';

      // Build our subquery first.
      $join_query = $this->connection->select('redhen_connection', 'sub');
      $join_query->addField('sub', 'id');
      $join_query->addField('sub', 'type');
      $join_query->addField('sub', $endpoint);
      $join_query->addField('sub', $shared_endpoint);
      $join_query->condition('sub.' . $endpoint, $entity2->id());

      // Base table is filtered on the first entity's id.
      $query = $this->connection->select('redhen_connection', 'c');
      $query->addField('c', 'id');
      $query->condition('c. '. $endpoint, $entity->id());

      // If we're filtering on active connections (default) limit status.
      if ($active) {
        $join_query->condition('sub.status', 1);
        $query->condition('c.status', 1);
      }

      // Join on type and endpoint match. Can't pass $endpoint as argument
      // because it will be automatically wrapped in quotes and break the SQL.
      $query->innerJoin($join_query, 'c2', 'c.type = c2.type AND c.' . $shared_endpoint . ' = c2.' . $shared_endpoint);

      $result = $query->execute();

      $type_results = $result->fetchCol(0);
      $results = array_merge($results, $type_results);
    }

    $connections = [];
    if (!empty($results)) {
      $connections = Connection::loadMultiple($results);
    }

    // This is returning the specific connection.
    // @todo for reference connections it needs to check for plugins and determine if a connection would be made by another entity type
    // because at this point we have a node, but the connection is to an org.
    return $connections;
  }

  /**
   * {@inheritdoc}
   */
  public function checkConnectionPermission(EntityInterface $endpoint1, $endpoint2, $operation, $permission_key) {
    $connections = $this->getConnections($endpoint1, $endpoint2);
    foreach ($connections as $connection) {
      /** @var ConnectionInterface $connection */
      $role = $connection->get('role')->entity;
      if (!$role) {
        return FALSE;
      }
      $permissions = $role->get('permissions');
      if (is_array($permissions[$permission_key]) && in_array($operation, $permissions[$permission_key])) {
        return new AccessResultAllowed();
      }
    }
    return new AccessResultNeutral();
  }

  /**
   * {@inhertitdoc}
   */
  public function getAllConnectionEntityTypes() {
    // Load all connection types.
    $query = $this->entityQuery->get('redhen_connection_type');
    $results = $query->execute();

    $connection_types = [];
    $connection_entity_types = [];

    if (!empty($results)) {
      $connection_types = ConnectionType::loadMultiple($results);
    }

    foreach ($connection_types as $type) {
      $bundle1 = $type->getEndpointEntityTypeId(1);
      $bundle2 = $type->getEndpointEntityTypeId(2);
      $connection_entity_types[$bundle1] = $bundle1;
      $connection_entity_types[$bundle2] = $bundle2;
    }

    // Get all entity types.
    $all_entity_types = $this->entityTypeManager->getDefinitions();

    // Iterate over entity types and remove if not in any connection types.
    foreach ($all_entity_types as $key => $entity_type) {
      if (!array_key_exists($key, $connection_entity_types)) {
        unset($all_entity_types[$key]);
      }
    }
    return $all_entity_types;
  }

  /**
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity we're querying against.
   * @param \Drupal\Core\Entity\EntityInterface $entity2
   *   The second entity we're querying against.
   * @param null $connection_type
   *   Limit query to this connection type.
   * @param bool $active
   *   Only active connections.
   *
   * @return QueryInterface
   */
  private function buildQuery(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL, $active = TRUE) {
    $types = ($connection_type) ? [$connection_type => ConnectionType::load($connection_type)] : $this->getConnectionTypes($entity, $entity2);

    /** @var QueryInterface $query */
    $query = $this->entityQuery->get('redhen_connection');

    // Add condition for the connection status.
    if ($active) {
      $query->condition('status', 1);
    }

    if ($connection_type != NULL) {
      $query->condition('type', $connection_type);
    }

    // Endpoint conditions.
    $entities = [];
    $entity_type = $entity->getEntityType()->id();
    $entities[$entity_type][] = $entity;
    $entity_type2 = ($entity2) ? $entity2->getEntityType()->id() : NULL;
    if ($entity2) {
      $entities[$entity_type2][] = $entity2;
    }

    // Overall OR group of connection_type/endpoint groupings.
    $endpoints_group = $query->orConditionGroup();

    // @todo Might instead be able to query against endpoint_1.entity.type, etc.

    // Build endpoint groups.
    foreach ($types as $type => $connection_type) {
      /** @var ConnectionTypeInterface $connection_type */
      $endpoints = [];
      $endpoints[$entity_type] = $connection_type->getEndpointFields($entity_type);

      // Add condition for the connection_type.
      $condition_group = $query->andConditionGroup()
        ->condition('type', $type);

      // Working with 2 endpoints.
      if ($entity_type2) {
        $group = $query->andConditionGroup();
        $endpoints[$entity_type2] = $connection_type->getEndpointFields($entity_type2);

        foreach ($endpoints as $endpoint_type => $endpoint_fields) {
          for ($x=0; $x < count($endpoint_fields); $x++) {
            $group->condition($endpoint_fields[$x], $entities[$endpoint_type][$x]->id());
          }
          // Endpoints are of the same type so we need to add an additional
          // condition for the reverse structure.
          if ($x > 1) {
            $group2 = $query->orConditionGroup();
            for ($x=count($endpoint_fields) - 1; $x >= 0; $x--) {
              $group2->condition($endpoint_fields[$x], $entities[$endpoint_type][$x]->id());
            }
            $group = $query->orConditionGroup()
              ->condition($group)
              ->condition($group2);
          }
        }

        $condition_group->condition($group);
      }
      else {
        // Single entity.
        $condition_group = $query->orConditionGroup();
        foreach ($endpoints as $endpoint_type => $endpoint_fields) {
          for ($x = 0; $x < count($endpoint_fields); $x++) {
            $condition_group->condition($endpoint_fields[$x], $entity->id());
          }
        }
      }
      $endpoints_group->condition($condition_group);
    }
    if (!empty($types)) {
      $query->condition($endpoints_group);
      return $query;
    }

    return FALSE;
  }

}
