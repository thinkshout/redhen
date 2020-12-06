<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Access\AccessResultAllowed;
use Drupal\Core\Access\AccessResultNeutral;
use Drupal\Core\Database\Connection as DBConnection;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\redhen_connection\Entity\ConnectionType;
use Drupal\redhen_connection\Entity\Connection;

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
   * @param \Drupal\Core\Database\Connection $connection
   *   The database connection.
   */
  public function __construct(EntityTypeManagerInterface $entity_type_manager, DBConnection $connection) {
    $this->entityTypeManager = $entity_type_manager;
    $this->connection = $connection;
  }

  /**
   * {@inheritdoc}
   */
  public function getConnectionEntityTypes(array $entity_types) {
    $all_connection_types = [];
    foreach ($entity_types as $entity_type => $type) {
      $query = $this->entityTypeManager->getStorage('redhen_connection_type')->getQuery();
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
    $query = $this->entityTypeManager->getStorage('redhen_connection_type')->getQuery();
    $or_group = $query->orConditionGroup();
    $entity_type = $entity->getEntityTypeId();

    if (empty($entity2)) {
      // Single entity provided.
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
  public function getConnections(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL, $active = TRUE) {
    $connections = [];
    $connections_matches = $this->connectionQuery($entity, $entity2, $connection_type, $active);

    if (!empty($connections_matches)) {
      $connections = Connection::loadMultiple($connections_matches);
    }

    return $connections;
  }

  /**
   * {@inheritdoc}
   */
  public function getConnectionCount(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL) {
    $connections = $this->connectionQuery($entity, $entity2, $connection_type);

    return count($connections);
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
   * {@inheritDoc}
   */
  public function checkConnectionPermission(EntityInterface $endpoint1, EntityInterface $endpoint2, $operation, $permission_key) {
    $connections = $this->getConnections($endpoint1, $endpoint2);
    foreach ($connections as $connection) {
      /** @var \Drupal\redhen_connection\Entity\ConnectionInterface $connection */
      $role = $connection->get('role')->entity;
      if (!$role) {
        return new AccessResultNeutral();
      }
      $permissions = $role->get('permissions');
      if (is_array($permissions[$permission_key]) && in_array($operation, $permissions[$permission_key])) {
        return new AccessResultAllowed();
      }
    }
    return new AccessResultNeutral();
  }

  /**
   * {@inheritDoc}
   */
  public function getAllConnectionEntityTypes() {
    // Load all connection types.
    $query = $this->entityTypeManager->getStorage('redhen_connection_type')->getQuery();
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
   * Query for connections.
   *
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity we're querying against.
   * @param \Drupal\Core\Entity\EntityInterface $entity2
   *   The second entity we're querying against.
   * @param string $connection_type
   *   Limit query to this connection type.
   * @param bool $active
   *   Only active connections.
   *
   * @return array
   *   An array of matches.
   */
  private function connectionQuery(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL, $active = TRUE) {
    $entity_type = $entity->getEntityType()->id();
    $entity2_type = ($entity2) ? $entity2->getEntityType()->id() : NULL;
    $connections_matches = [];
    $potential_endpoints = [];

    if (!$connection_type) {
      $connection_types = $this->getConnectionTypes($entity, $entity2);
    }
    else {
      $connection_types = [ConnectionType::load($connection_type)];
    }

    if (!empty($connection_types)) {
      foreach ($connection_types as $type) {
        if ($endpoint_fields = $type->getEndpointFields($entity_type)) {
          $potential_endpoints[$type->id()]['entity1'] = $endpoint_fields;
        }

        if ($entity2_type) {
          if ($endpoint2_fields = $type->getEndpointFields($entity2_type)) {
            $potential_endpoints[$type->id()]['entity2'] = $endpoint2_fields;
          }
        }
      }

      $database = \Drupal::database();

      foreach ($potential_endpoints as $connection_type => $endpoint_group) {

        $query = $database->select('redhen_connection', 'rc')
          ->fields('rc', ['id'])
          ->condition('type', $connection_type);

        if ($active) {
          $query->condition('status', $active);
        }

        // Parent condition group.
        $entityAndGroup = $query->andConditionGroup();

        // Entity 1 Group.
        $entity1Group = $query->orConditionGroup();
        $entity1Group->condition($endpoint_group['entity1'][0], $entity->id());

        // If there are multiple potential endpoints that match entity 1 type.
        if (count($endpoint_group['entity1']) > 1) {
          $additional_entities = array_slice($endpoint_group['entity1'], 1, 1, FALSE);
          $entity1Group->condition($additional_entities[0], $entity->id());
        }

        $entityAndGroup->condition($entity1Group);

        // Entity 2 Group.
        if (isset($endpoint_group['entity2'])) {
          $entity2Group = $query->orConditionGroup()
            ->condition($endpoint_group['entity2'][0], $entity2->id());

          // If there are multiple potential endpoints that match entity 2 type.
          if (isset($endpoint_group['entity2'][1])) {
            $entity2Group->condition($endpoint_group['entity2'][1], $entity2->id());
          }

          $entityAndGroup->condition($entity2Group);
        }

        $query->condition($entityAndGroup);
        $results = $query->execute()->fetchCol();

        // If there are matched results merge them into the result set.
        if ($results) {
          $connections_matches = array_unique(array_merge($connections_matches, $results));
        }
      }
    }

    return $connections_matches;
  }

}
