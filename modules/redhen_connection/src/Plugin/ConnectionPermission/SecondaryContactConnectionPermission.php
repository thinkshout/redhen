<?php

namespace Drupal\redhen_connection\Plugin\ConnectionPermission;

use Drupal\Core\Access\AccessResultNeutral;
use Drupal\redhen_contact\Entity\Contact;
use Drupal\redhen_connection\Plugin\ConnectionPermissionBase;
use Drupal\redhen_connection\Plugin\ConnectionPermissionInterface;
use Drupal\Core\Entity\EntityInterface;

/**
 * Provide permission for contacts that share a redhen_connection.
 *
 * @ConnectionPermission(
 *  id = "secondary_contact_connection_permission",
 *  label = @Translation("Secondary Contact"),
 *  description = @Translation("A contact connected to the same entity via connection of the same type. Sitewide permissions will override this setting."),
 *  subject_entity_type = "redhen_contact",
 *  subject_entity_bundle = "",
 *  influencer_entity_type = "redhen_org",
 * )
 */
class SecondaryContactConnectionPermission extends ConnectionPermissionBase implements ConnectionPermissionInterface {

  /**
   * {@inheritdoc}
   */
  public function getPermissionKey() {
    return 'contact';
  }

  /**
   * {@inheritdoc}
   */
  public function getInfluencers(EntityInterface $subject_entity) {
    $influencers = [];
    // Get all connection types for this subject.
    $connection_types = $this->redhenConnectionConnections->getConnectionTypes($subject_entity);
    // Loop over types and find contact and org connection types.
    foreach ($connection_types as $type) {
      $endpoints[] = $type->getEndpointEntityTypeId(1);
      $endpoints[] = $type->getEndpointEntityTypeId(2);
      // If both there is a contact and an org endpoint take further action.
      if (in_array($this->get('subject_entity_type'), $endpoints) && in_array($this->get('influencer_entity_type'), $endpoints)) {
        // Load up the org from the connection.
        $connected_entities = $this->redhenConnectionConnections->getConnectedEntities($subject_entity, $type->id());
        // Add the connected entities to the influencers array.
        foreach ($connected_entities as $entity) {
          $influencers[] = $entity;
        }
      }
    }
    return $influencers;
  }

  /**
   * {@inheritdoc}
   */
  public function hasRolePermissions(EntityInterface $subject_entity, $operation, Contact $contact) {
    $access = new AccessResultNeutral();;

    $influencers = $this->getInfluencers($subject_entity);
    foreach ($influencers as $influencer) {
      $access = $this->redhenConnectionConnections->checkConnectionPermission($contact, $influencer, $operation, $this->getPermissionKey());
      if ($access->isAllowed()) {
        return $access;
      }
    }
    return $access;
  }

}
