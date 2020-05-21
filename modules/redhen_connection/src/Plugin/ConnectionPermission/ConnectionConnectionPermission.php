<?php

namespace Drupal\redhen_connection\Plugin\ConnectionPermission;

use Drupal\Core\Access\AccessResultNeutral;
use Drupal\redhen_connection\Entity\ConnectionType;
use Drupal\redhen_contact\Entity\Contact;
use Drupal\redhen_connection\Plugin\ConnectionPermissionBase;
use Drupal\redhen_connection\Plugin\ConnectionPermissionInterface;
use Drupal\Core\Entity\EntityInterface;


/**
 * @ConnectionPermission(
 *  id = "connection_connection_permission",
 *  label = @Translation("Connection"),
 *  description = @Translation("Applies to both current users connection and secondary connections. Sitewide permissions will override this permission."),
 *  subject_entity_type = "redhen_connection",
 *  subject_entity_bundle = "",
 *  influencer_entity_type = "",
 * )
 */
class ConnectionConnectionPermission extends ConnectionPermissionBase implements ConnectionPermissionInterface{

    /** @var \Drupal\Core\Entity\EntityInterface */
    private $contact;

    /**
     * {@inheritdoc}
     */
    public function getPermissionKey() {
      return 'connection';
    }

    /**
     * {@inheritdoc}
     */
    public function getInfluencers(EntityInterface $subject_entity) {
      // Return the connections for the current contact and any
      return $this->redhenConnectionConnections->getConnectedEntities($this->contact, $subject_entity->getType());
    }

    /**
     * {@inheritdoc}
     */
    public function hasRolePermissions(EntityInterface $subject_entity, $operation, Contact $contact) {
      $this->contact = $contact;
      // Only check permissions for connections that are of a type with contacts.
      $connection_type = ConnectionType::load($subject_entity->getType());
      if ($connection_type->getEndpointEntityTypeId(1) == 'redhen_contact' || $connection_type->getEndpointEntityTypeId(2) == 'redhen_contact') {
        //
        $influencers = $this->getInfluencers($subject_entity);
        if ($influencers) {
          $influencer = reset($influencers);
          return $this->redhenConnectionConnections->checkConnectionPermission($contact, $influencer, $operation, $this->getPermissionKey());
        }
      }

      return new AccessResultNeutral();
    }

}
